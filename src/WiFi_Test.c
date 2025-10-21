/*
 * WiFi_Test.c
 *
 *  Created on: Apr 30, 2025
 *      Author: michaeljacobson
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <glib-2.0/glib.h>
#include <libnm/NetworkManager.h>
#include "WiFiChannelDef.h"

#define WLAN0_IFCONFIG_UP						"ifconfig wlan0 up"
#define WPA_SUPPLICANT_INIT_COMMAND             "systemctl restart wpa_supplicant"
#define WIFI_SSID_STR_LEN                    	100U
#define WIFI_PASSWORD_STR_LEN                	100U
#define WIFI_IP_ADDR_STR_LEN           			16U                        /* 000.000.000.000 + NULL */
#define WIFI_PORT_STR_LEN              			6U                         /* 5 characters + NULL (0 to 65535) */
#define WIFI_MAC_ADDR_STR_LEN             		18U
#define WIFI_CHANNEL_STR_LEN           			3U                         /* 2 characters + NULL (1 to 14) */
#define WIFI_LIST_SIZE                          50U
#define INVALID_NET_ID                          0xFF
#define WIFI_SCAN_COMMAND                       "SCAN"
#define WIFI_SCAN_RESULTS_COMMAND               "SCAN_RESULTS"

typedef enum
{
   WIFI_SECURITY_INVALID=0,
   WIFI_SECURITY_OPEN,
   WIFI_SECURITY_WEP,
   WIFI_SECURITY_WPA,
   WIFI_SECURITY_WPA2,
   WIFI_SECURITY_WPA3
} WiFiSecurityType_t;

typedef enum
{
    WIFI_CONNECT_SETUP_SEND_SSID,
    WIFI_CONNECT_SETUP_SEND_PSK_TYPE,
    WIFI_CONNECT_SETUP_SEND_PSK,
    WIFI_CONNECT_SETUP_SEND_SELECT,
    WIFI_CONNECT_SETUP_SEND_ENABLE
} WiFiSetupCommandStates_t;

typedef struct
{
   char SSIDName[WIFI_SSID_STR_LEN];
   char PassCode[WIFI_PASSWORD_STR_LEN];
   char MACAddr[WIFI_MAC_ADDR_STR_LEN+1];
   //WiFiSecurityType_t SecurityType;
   char SecurityType[16];
   char RecentlyUsed;
   short net_id;
   short Extra2;
   int Extra3;
   int Extra4;
} SavedWiFiNetworkHeader_t;

typedef struct
{
   SavedWiFiNetworkHeader_t Header;
   char Channel[WIFI_CHANNEL_STR_LEN];
   char Power;
   char Saved;
   char Index;
   char BSSID[WIFI_MAC_ADDR_STR_LEN];
} WiFiNetwork_t;


typedef struct
{
    int length;
    WiFiNetwork_t network[WIFI_LIST_SIZE];
} wifi_network_list_t;

typedef struct
{
   WiFiNetwork_t Router;
    char message[4096];
    int index;
    int returnValue;
} WiFiParams_t;


pthread_t CallbacksThread = 0;


wifi_network_list_t g_wifi_scan_list;

NMClient *p_client = NULL;
GError *p_error = NULL;
NMDevice *p_dev = NULL;
NMDeviceWifi *p_wifi = NULL;
gint64 previous_scan_time_in_ms = 0;
GCancellable scan_cancellable;
NMConnection *p_connection = NULL;
GMainLoop *p_active_connection_loop = NULL;
NMActiveConnection *p_ActiveConnection = NULL;
NMRemoteConnection *p_remote_connection = NULL;

pthread_t WiFiSignalThread = 0;

int MaxNetworks = 0;

void CloseWiFi(void);

static char CheckWiFiServiceStarted(void);
static void GetChannelFromFreq(guint32 freq, char *p_outputString);
void DisconnectFromWiFi(void);
void GetWiFiStatus(void);
void DecodeAndPrintActiveConnectionStateAndReason(NMActiveConnectionState state, NMActiveConnectionStateReason reason);

void WiFiScanCB(GObject *p_source_object, GAsyncResult *p_result, gpointer user_data);
void NewConnectionCB(GObject *object, GAsyncResult *res, gpointer user_data);
void ActiveConnectionDisconnectCB(GObject *source_object, GAsyncResult *res, gpointer user_data);
void RemoteConnectionDeleteCB(GObject *source_object, GAsyncResult *res, gpointer user_data);
void *WiFiSignalCallbacks(void *arg);
void ActiveConnectionStateChangedCB(NMActiveConnection *p_active_connection, guint state, guint reason, gpointer user_data);

/*****************************************************************************
 *
 */
char InitWiFi(void)
{
	char return_val = 0;

	//first check if the service is running
	if(CheckWiFiServiceStarted() == 0)
	{
		p_client = nm_client_new(NULL, &p_error);
		if (p_client == NULL)
		{
			g_message("Error: Could not connect to NetworkManager: %s.", p_error->message);
			g_error_free(p_error);
			p_error = NULL;
			return -1;
		}
		else
		{
			p_dev = nm_client_get_device_by_iface(p_client, "wlan0");

			if(p_dev != NULL)
			{
				p_wifi = NM_DEVICE_WIFI(p_dev);

				if(p_wifi != NULL)
				{
					g_object_ref(p_wifi);
					g_object_ref(p_dev);
					g_object_ref(p_client);
					pthread_create( &WiFiSignalThread, NULL, WiFiSignalCallbacks, NULL );
					return_val = 1;
				}
				else
				{
					printf("WiFi Device not found for wlan0 device\n");
				}
			}
			else
			{
				printf("device not found for wlan0\n");
			}
		}
	}
	else
	{
		printf("WiFi Service not running\n");
	}

	return return_val;
}

/*****************************************************************************
 *
 */
void CloseWiFi(void)
{
	DisconnectFromWiFi();

	if(p_active_connection_loop)
	{
		g_main_loop_quit(p_active_connection_loop);

		if(WiFiSignalThread)
		{
			pthread_join(WiFiSignalThread, NULL);
		}

		if(p_active_connection_loop)
		{
			g_main_loop_unref(p_active_connection_loop);
		}
	}
	if(p_connection)
	{
		g_object_unref(p_connection);
	}
	if(p_ActiveConnection)
	{
		g_object_unref(p_ActiveConnection);
	}
	if(p_remote_connection)
	{
		g_object_unref(p_remote_connection);
	}
	if(p_wifi)
	{
		g_object_unref(p_wifi);
	}
	if(p_dev)
	{
		g_object_unref(p_dev);
	}
	if(p_error)
	{
		g_error_free(p_error);
		p_error = NULL;
	}
	if(p_client)
	{
		g_object_unref(p_client);
	}
}

/*****************************************************************************
 * this is for the active connection signal, but we can't add the signal until we get an active connection
 */
void *WiFiSignalCallbacks(void *arg)
{
	GMainContext *p_SignalsContext = g_main_context_new (); //new thread means new context or else nothing else will run or we will get g errors

	if((p_client != NULL) && (p_SignalsContext != NULL))
	{
		//printf("Starting Signal Callbacks\n");
		p_active_connection_loop = g_main_loop_new(p_SignalsContext, FALSE);

		g_main_loop_run(p_active_connection_loop);

		g_main_loop_unref(p_active_connection_loop);
		p_active_connection_loop = NULL;

		//printf("Ending Signal Callbacks\n");
	}
	else
	{
		printf("Cannot start signals\n");
	}

	return 0;
}


/*****************************************************************************
 * scans do not happen immediately, we live on the old scan until the new one happens
 * and that is only detectable by the timestamp of the last scan
 */
void DoWiFiScan(void)
{
	char do_it_again = 0;

	//printf("Doing WiFi Scan\n");

	memset((void *)&g_wifi_scan_list, 0, sizeof(g_wifi_scan_list));

	//current_time_in_ms = nm_utils_get_timestamp_msec();
	previous_scan_time_in_ms = nm_device_wifi_get_last_scan (p_wifi);

	//printf("Pre-scan last scan: %ld\n", previous_scan_time_in_ms);

	do
	{
		//get current time stamp
		if(previous_scan_time_in_ms > 0)
		{
			gint64 this_scan = 0;
			GMainLoop *p_scan_loop;

			//printf("starting scan async\n");
			p_scan_loop = g_main_loop_new(NULL, FALSE);
			//request scan  - returns immediately, immediately calls callback
			nm_device_wifi_request_scan_async (p_wifi, NULL, &WiFiScanCB, p_scan_loop);

			g_main_loop_run(p_scan_loop);

			g_main_loop_unref(p_scan_loop);

			this_scan = nm_device_wifi_get_last_scan (p_wifi);

			if(this_scan == previous_scan_time_in_ms)
			{
				//we need to do this again
				do_it_again = 1;
				printf("scan failed after many attempts, trying again\n");
			}
			else
			{
				do_it_again = 0;
			}
		}
	}while(do_it_again);

	printf("Done Scanning\n");
}



/*****************************************************************************
 *
 */
void WiFiScanCB(GObject *p_source_object, GAsyncResult *p_result, gpointer user_data)
{
	gint64 this_scan_in_ms = 0;
	int timeout = 0;
	NMDeviceWifi *p_ThisWifi  = NM_DEVICE_WIFI(p_source_object);
	GMainLoop *p_mainLoop = (GMainLoop *)(user_data);
	int try_again = 0;

	if(p_wifi == p_ThisWifi)
	{
		do
		{
			//get scan results
			if(nm_device_wifi_request_scan_finish (p_ThisWifi, p_result, &p_error) == TRUE)
			{
				this_scan_in_ms = nm_device_wifi_get_last_scan (p_ThisWifi);
				//printf("Post-scan last scan: %ld\n", this_scan_in_ms);

				if((this_scan_in_ms > previous_scan_time_in_ms) )
				{
					printf("Get APs\n");
					const GPtrArray *p_aps = nm_device_wifi_get_access_points(p_wifi);

					if (p_aps != NULL)
					{
						MaxNetworks = p_aps->len;

						printf("getting %d Networks\n\n", MaxNetworks);
						for (guint i = 0; i < MaxNetworks; i++)
						{
							int success = 0;
							guint32 freq = 0;
							guint8 power = 0;
							printf("Array index: %d\n", i);
							NMAccessPoint *p_ap = (NMAccessPoint *)g_ptr_array_index(p_aps, i);
							printf("got AP%d\n", i);
							if (p_ap != NULL)
							{
								//printf("Get ssid\n");
								GBytes *p_data_bytes = nm_access_point_get_ssid(p_ap);
								if (p_data_bytes != NULL)
								{
									char *p_ssid_str = nm_utils_ssid_to_utf8(g_bytes_get_data(p_data_bytes, NULL), g_bytes_get_size(p_data_bytes));
									if(p_ssid_str != NULL)
									{
										strcpy(g_wifi_scan_list.network[i].Header.SSIDName, p_ssid_str);
										success++;
										g_free(p_ssid_str);
									}
									//g_bytes_unref(p_data_bytes); do not do this. it will release the memory for the AP, not some string, so a second scan will crash
								}
								else
								{
									strcpy(g_wifi_scan_list.network[i].Header.SSIDName, "#########");
									success++;
									//printf("Fail SSID\n");
								}


								//printf("Get bssid\n");
								const char *p_bssid_bytes = nm_access_point_get_bssid(p_ap);
								if (p_bssid_bytes != NULL)
								{
									strcpy(g_wifi_scan_list.network[i].BSSID, p_bssid_bytes);
									success++;
								}
								else
								{
									printf("Fail BSSID\n");
								}

								//printf("Get strength\n");
								power = nm_access_point_get_strength(p_ap); //% of strnegth 0-100
								if (power != 0)
								{
									g_wifi_scan_list.network[i].Power = power;
									success++;
								}
								else
								{
									printf("Fail Strength\n");
								}

								//printf("Get flags\n");
								NM80211ApSecurityFlags wpa_flags = nm_access_point_get_wpa_flags(p_ap);
								NM80211ApSecurityFlags rsn_flags = nm_access_point_get_rsn_flags(p_ap);

								if ((wpa_flags & NM_802_11_AP_SEC_KEY_MGMT_PSK) || (rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_PSK))
								{
									strcpy(g_wifi_scan_list.network[i].Header.SecurityType, "wpa-psk");
									success++;
								}
								else if ((wpa_flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X) || (rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X))
								{
									strcpy(g_wifi_scan_list.network[i].Header.SecurityType, "wpa-eap");
									success++;
								}
								else if ((wpa_flags & NM_802_11_AP_SEC_KEY_MGMT_SAE) || (rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_SAE))
								{
									strcpy(g_wifi_scan_list.network[i].Header.SecurityType, "sae");
									success++;
								}
								else if ((wpa_flags == 0) && (rsn_flags == 0))
								{
									strcpy(g_wifi_scan_list.network[i].Header.SecurityType, "wpa-none");
									success++;
								}
								else
								{
									strcpy(g_wifi_scan_list.network[i].Header.SecurityType, "Unknown");
									success++;
								}

								//printf("get freq\n");
								freq = nm_access_point_get_frequency(p_ap);
								if(freq > 0)
								{
									GetChannelFromFreq(freq, g_wifi_scan_list.network[i].Channel);
									success++;
								}
								else
								{
									printf("Fail Freq\n");
								}

								if(success == 5)
								{
									printf("AP%d: SSID: %s, BSSID: %s, Strength: %d, Channel: %s, Security: %s\n",
											i,
											g_wifi_scan_list.network[i].Header.SSIDName,
											g_wifi_scan_list.network[i].BSSID,
											g_wifi_scan_list.network[i].Power,
											g_wifi_scan_list.network[i].Channel,
											g_wifi_scan_list.network[i].Header.SecurityType);
								}
							}
							else
							{
								printf("Array index returned NULL\n");
							}
						}

						break;
					}
					else
					{
						printf("could not get APs\n");
					}
				}
				else
				{
					printf("Not a new scan, sleeping\n");
					sleep(4);
					nm_device_wifi_request_scan_async(p_ThisWifi, NULL, WiFiScanCB, user_data);

					try_again++;

					if(try_again >= 10)
					{
						g_main_loop_quit(p_mainLoop);
					}
					return;
				}
			}
			else
			{
				g_message("failed scan finish: %s\n", p_error->message);
				g_error_free(p_error);
				p_error = NULL;
			}

			break;

			timeout += 100;

			usleep(100*1000);
		} while(timeout < 15*1000); //15s timeout
	}
	else
	{
		printf("wifi or user data do not match\n");
	}

	printf("Callback done\n");

	g_main_loop_quit(p_mainLoop);

}



/*******************************************************************************************************************************
 * static BOOL CheckWiFiServiceStarted(void)
 *
 *  checks if wpa_supplicant is up and running by running
 *
 * Parameters:
 *      - none
 *
 * Returns:
 *      - 0 is initialized, 0 < otherwise
 *
 * Notes:
 *
 *******************************************************************************************************************************/
static char CheckWiFiServiceStarted(void)
{
    FILE *fp;
    char command[100];
    char buf[100];
    char retVal = 2;

    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "systemctl status wpa_supplicant | grep 'active (running)'");

    while(retVal > 0)
    {
		// run the command and save the output to a file
		if ((fp = popen(command, "r")) == 0)
		{
			printf("Error opening pipe!\n");
		}
		else
		{
			// go through file line by line to parse scan command output
			while (fgets(buf, sizeof(buf), fp) != 0)
			{
				if (strstr(buf, "active (running) since"))
				{
					// Platform Manager is up and running
					//printf("systemd wpa_supplcant service running\n");
					retVal = 0;
					break;
				}
				else if(retVal == 2)
				{
					printf("wpa_supplicant is not running\n");
					retVal--;

					system(WLAN0_IFCONFIG_UP);
					system(WPA_SUPPLICANT_INIT_COMMAND);
				}
				else
				{
					retVal = -1;
				}
			}

			if (pclose(fp) == -1)
			{
				printf("Error closing pipe!\n");
			}
		}
    }

    return retVal;
}

/********************************************************************************************************************
 *
 ********************************************************************************************************************/
static void GetChannelFromFreq(guint32 freq, char *p_outputString)
{
	unsigned char Channel = 0;
	int i = 0;

	/*
	 * according to this page:https://en.wikipedia.org/wiki/List_of_WLAN_channels
	 * i am looking for the Fundamental frequency for the channels. i understand the
	 * channels are a range, so if it deviates from the fundamental freq then i have to
	 * start looking inside the ranges
	 */
	while(WiFiChannelDef[i].FundFreq != 0)
	{
		if(freq == WiFiChannelDef[i].FundFreq)
		{
			Channel = WiFiChannelDef[i].Channel;
			break;
		}

		i++;
	}

	if(Channel == 0)
	{
		//we did not find a fundamental freq, so go looking in the ranges
		i = 0;
		while(WiFiChannelDef[i].FreqRange_Lo != 0)
		{
			if((freq >= WiFiChannelDef[i].FreqRange_Lo) && (freq <= WiFiChannelDef[i].FreqRange_Hi))
			{
				Channel = WiFiChannelDef[i].Channel;
				break;
			}

			i++;
		}
	}

	if(Channel == 0)
	{
		//we still find the channel?
		p_outputString[0] = '?';
	}
	else
	{
		sprintf(p_outputString, "%d", Channel);
	}
}

/********************************************************************************************************************
 *
 ********************************************************************************************************************/
void ConnectToWiFiNetwork(int index)
{
	printf("Attmpting connection...\n");
	if(index < MaxNetworks)
	{
		GMainLoop *p_connect_loop;
		// Create a new connection profile
		p_connection = nm_simple_connection_new();

		// Create and set the connection settings
		NMSettingConnection *p_connectionSetting = NM_SETTING_CONNECTION(nm_setting_connection_new());
		g_object_set(G_OBJECT(p_connectionSetting),
					 NM_SETTING_CONNECTION_ID, g_wifi_scan_list.network[index].Header.SSIDName,
					 NM_SETTING_CONNECTION_UUID, nm_utils_uuid_generate(),
					 NM_SETTING_CONNECTION_TYPE, NM_SETTING_WIRELESS_SETTING_NAME,
					 NM_SETTING_CONNECTION_AUTOCONNECT, FALSE,
					 NULL);
		nm_connection_add_setting(p_connection, NM_SETTING(p_connectionSetting));

		// Create and set the wireless settings
		NMSettingWireless *p_wirelessSetting = NM_SETTING_WIRELESS(nm_setting_wireless_new());

		GBytes *p_ssid = g_bytes_new(g_wifi_scan_list.network[index].Header.SSIDName, strlen(g_wifi_scan_list.network[index].Header.SSIDName));
		GBytes *p_bssid = g_bytes_new(g_wifi_scan_list.network[index].BSSID, strlen(g_wifi_scan_list.network[index].BSSID));
		g_object_set(G_OBJECT(p_wirelessSetting),
					 NM_SETTING_WIRELESS_SSID, p_ssid,
					 NM_SETTING_WIRELESS_BSSID, p_bssid,
					 NM_SETTING_WIRELESS_MODE, NM_SETTING_WIRELESS_MODE_INFRA,
					 NULL);
		nm_connection_add_setting(p_connection, NM_SETTING(p_wirelessSetting));

		/*
		 * Key Management Options
		"wpa-psk": This is a common option used for Wi-Fi networks that use a Pre-Shared Key (PSK) for authentication. This applies to WPA, WPA2, and increasingly WPA3-Personal networks.

		"sae": This option is for WPA3-Personal networks, which use Simultaneous Authentication of Equals (SAE) for more secure authentication than traditional WPA-PSK.

		"wpa-eap": This option is used for enterprise-level Wi-Fi security, where a central authentication server (like RADIUS) is used via the Extensible Authentication Protocol (EAP) to authenticate users.

		"wpa-none": This indicates no security is used for the network. While it exists, it's generally not recommended for security.
		 */
		// Create and set the wireless security settings
		NMSettingWirelessSecurity *p_wirelessSecuritySetting = NM_SETTING_WIRELESS_SECURITY(nm_setting_wireless_security_new());
		if(strstr(g_wifi_scan_list.network[index].Header.SecurityType, "none") == NULL)
		{
			printf("Security: %s Setting PW: %s\n", g_wifi_scan_list.network[index].Header.SecurityType, g_wifi_scan_list.network[index].Header.PassCode);
			if(g_wifi_scan_list.network[index].Header.PassCode[0] != 0)
			{
				g_object_set(G_OBJECT(p_wirelessSecuritySetting),
							 NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, g_wifi_scan_list.network[index].Header.SecurityType,
							 NM_SETTING_WIRELESS_SECURITY_PSK, g_wifi_scan_list.network[index].Header.PassCode,
							 NULL);
				nm_connection_add_setting(p_connection, NM_SETTING(p_wirelessSecuritySetting));
			}
			else
			{
				printf("No password found for %s\n", g_wifi_scan_list.network[index].Header.SSIDName);
			}
		}
		else
		{
			printf("No Security\n");
		}

		// Add IP settings (e.g., DHCP)
		NMSettingIP4Config *p_ip4 = (NMSettingIP4Config *)nm_setting_ip4_config_new();
		g_object_set(G_OBJECT(p_ip4),
					 NM_SETTING_IP_CONFIG_METHOD,
					 NM_SETTING_IP4_CONFIG_METHOD_AUTO,
					 NULL);
		nm_connection_add_setting(p_connection, NM_SETTING(p_ip4));

		// Increment reference counts for connection and client objects
		// to prevent premature destruction during asynchronous operation
		g_object_ref(p_connection);

		p_connect_loop = g_main_loop_new(NULL, FALSE);


		nm_client_add_and_activate_connection_async(p_client, p_connection, p_dev, NULL, NULL, NewConnectionCB, p_connect_loop);

		// Start the main loop to process asynchronous operations
		g_main_loop_run(p_connect_loop);
		g_main_loop_unref(p_connect_loop);
	}
	else
	{
		printf("Index outside range of networks\n");
	}
}

/********************************************************************************************************************
 *
 ********************************************************************************************************************/
void NewConnectionCB(GObject *object, GAsyncResult *res, gpointer user_data)
{
	GMainLoop *p_mainLoop = (GMainLoop *)(user_data);

	p_ActiveConnection = nm_client_add_and_activate_connection_finish(p_client, res, &p_error);

	if(p_ActiveConnection == NULL)
	{
		printf("No active connection found yet\n");
		sleep(1);
		nm_client_add_and_activate_connection_async(p_client, p_connection, p_dev, NULL, NULL, NewConnectionCB, user_data);
		return;
	}
	else if (p_error)
	{
		g_message("Error adding and activating connection: %s\n", p_error->message);
		g_error_free(p_error);
		p_error = NULL;
	}
	else
	{
		printf("Connection added and activated successfully! -->This doesn't mean we are connected.\n");
		g_signal_connect(p_ActiveConnection, "state-changed", G_CALLBACK(ActiveConnectionStateChangedCB), p_active_connection_loop);

		GetWiFiStatus();
	}

	g_main_loop_quit(p_mainLoop);
}

/********************************************************************************************************************
 *
 ********************************************************************************************************************/
void ActiveConnectionStateChangedCB(NMActiveConnection *p_active_connection, guint state, guint reason, gpointer user_data)
{
	DecodeAndPrintActiveConnectionStateAndReason(state, reason);
}

/********************************************************************************************************************
 *
 ********************************************************************************************************************/
void SetPasswordForWiFiNetwork(int index, char *p_password)
{
	if((index < MaxNetworks) && (p_password != NULL))
	{
		if(strlen(p_password) < WIFI_PASSWORD_STR_LEN)
		{
			printf("Setting WiFi %d Passcode: %s\n", index, p_password);
			strcpy(g_wifi_scan_list.network[index].Header.PassCode, p_password);
		}
	}
}

/********************************************************************************************************************
 *
 ********************************************************************************************************************/
void GetWiFiStatus(void)
{
	if (p_ActiveConnection == NULL)
	{
		g_print("No active WiFi connection found\n");
	}
	else
	{
		printf("Active wifi connection found\n");

		NMActiveConnectionState active_connection_state = nm_active_connection_get_state (p_ActiveConnection);
		NMActiveConnectionStateReason active_connection_reason = nm_active_connection_get_state_reason (p_ActiveConnection);

		DecodeAndPrintActiveConnectionStateAndReason(active_connection_state, active_connection_reason);

		system("ifconfig"); //verification from the system
	}
}

/********************************************************************************************************************
 *
 ********************************************************************************************************************/
void DecodeAndPrintActiveConnectionStateAndReason(NMActiveConnectionState state, NMActiveConnectionStateReason reason)
{
	printf("\n***");
	switch(state)
	{
		case NM_ACTIVE_CONNECTION_STATE_UNKNOWN:
			printf("Active Connection State: Unknown");
		break;

		case NM_ACTIVE_CONNECTION_STATE_ACTIVATING:
			printf("Active Connection State: Activating");
		break;

		case NM_ACTIVE_CONNECTION_STATE_ACTIVATED:
			printf("Active Connection State: Activated");
		break;

		case NM_ACTIVE_CONNECTION_STATE_DEACTIVATING:
			printf("Active Connection State: Deactivating");
		break;

		case NM_ACTIVE_CONNECTION_STATE_DEACTIVATED:
			printf("Active Connection State: Deactivated");
			g_signal_handlers_disconnect_by_func (p_ActiveConnection, ActiveConnectionStateChangedCB, p_active_connection_loop);
		break;

		default:
			printf("active state: none of the above");
		break;
	}

	printf(" ---> ");
	switch(reason)
	{
		case NM_ACTIVE_CONNECTION_STATE_REASON_UNKNOWN:
			printf("The reason for the active connection state change is unknown.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_NONE:
			printf("No reason was given for the active connection state change.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_USER_DISCONNECTED:
			printf("The active connection changed state because the user disconnected it.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_DEVICE_DISCONNECTED:
			printf("The active connection changed state because the device it was using was disconnected.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_SERVICE_STOPPED:
			printf("The service providing the VPN connection was stopped.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_IP_CONFIG_INVALID:
			printf("The IP config of the active connection was invalid.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_CONNECT_TIMEOUT:
			printf("The connection attempt to the VPN service timed out.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_SERVICE_START_TIMEOUT:
			printf("A timeout occurred while starting the service providing the VPN connection.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_SERVICE_START_FAILED:
			printf("Starting the service providing the VPN connection failed.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_NO_SECRETS:
			printf("Necessary secrets for the connection were not provided.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_LOGIN_FAILED:
			printf("Authentication to the server failed.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_CONNECTION_REMOVED:
			printf("The connection was deleted from settings.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_DEPENDENCY_FAILED:
			printf("Master connection of this connection failed to activate.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_DEVICE_REALIZE_FAILED:
			printf("Could not create the software device link.\n");
		break;


		case NM_ACTIVE_CONNECTION_STATE_REASON_DEVICE_REMOVED:
			printf("The device this connection depended on disappeared.\n");
		break;

		default:
			printf("Who-knows? reason\n");
		break;
	}

	printf("\n");
}

/********************************************************************************************************************
 *
 ********************************************************************************************************************/
void DisconnectFromWiFi(void)
{
	if(p_ActiveConnection != NULL)
	{
		NMActiveConnectionState current_state = nm_active_connection_get_state (p_ActiveConnection);
		if((current_state == NM_ACTIVE_CONNECTION_STATE_ACTIVATED) || (current_state == NM_ACTIVE_CONNECTION_STATE_ACTIVATING))
		{
			GMainLoop *p_disconnect_loop = g_main_loop_new(NULL, FALSE);

			//get some kind of handle on how to get the store wifi connection so we can delete it
			p_remote_connection = nm_client_get_connection_by_uuid (p_client, nm_active_connection_get_uuid (p_ActiveConnection));

			//g_signal_handlers_disconnect_by_func (p_ActiveConnection, ActiveConnectionStateChangedCB, p_active_connection_loop); this has to happen before we d/c

			//shut down current wifi connection
			nm_client_deactivate_connection_async(p_client, p_ActiveConnection, NULL, ActiveConnectionDisconnectCB, p_disconnect_loop);

			g_main_loop_run(p_disconnect_loop);

			//wait for it to return and delete the stored/remote connection
			g_object_unref(p_ActiveConnection);

			//delete saved/remote connection if it exists
			if(p_remote_connection != NULL)
			{
				nm_remote_connection_delete_async (p_remote_connection, NULL, RemoteConnectionDeleteCB, p_disconnect_loop);

				g_main_loop_run(p_disconnect_loop);
			}

			//end that loop
			g_main_loop_unref(p_disconnect_loop);

			//invalidate the pointers
			//g_object_unref(p_remote_connection); i get an error when i do this, apparently this gets unref'd on delete, or we don't need to unref this

			p_ActiveConnection = NULL; //unref doesn't NULL the pointers
			p_remote_connection = NULL;
		}
		else
		{
			printf("state isn't activated\n");
		}
	}
	else
	{
		printf("Active Connection not available\n");
	}
}


/********************************************************************************************************************
 *
 ********************************************************************************************************************/
void ActiveConnectionDisconnectCB(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    gboolean success = nm_client_deactivate_connection_finish(p_client, res, &p_error);
    if (!success)
    {
        g_print("Error deactivating connection: %s\n", p_error->message);
        g_error_free(p_error);
		p_error = NULL;
    }
    else
    {
        g_print("WiFi connection disconnected successfully\n");
    }
    GMainLoop *loop = (GMainLoop *)user_data;
    g_main_loop_quit(loop);
}


/********************************************************************************************************************
 *
 ********************************************************************************************************************/
void RemoteConnectionDeleteCB(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
	gboolean success = nm_remote_connection_delete_finish (p_remote_connection, res, &p_error);

	if (!success)
	{
		g_print("Error deleting remote connection: %s\n", p_error->message);
		g_error_free(p_error);
		p_error = NULL;
	}
	else
	{
		g_print("WiFi remote connection deleted successfully\n");
	}

	GMainLoop *loop = (GMainLoop *)user_data;
	g_main_loop_quit(loop);
}






