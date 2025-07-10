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
//#include <wpa_ctrl.h>

#define WPA_SUPPLICANT_INIT_COMMAND             "systemctl enable wpa_supplicant-nl80211@wlan0.service"
#define WIFI_SSID_STR_LEN                    	100U
#define WIFI_PASSWORD_STR_LEN                	100U
#define WIFI_IP_ADDR_STR_LEN           			16U                        /* 000.000.000.000 + NULL */
#define WIFI_PORT_STR_LEN              			6U                         /* 5 characters + NULL (0 to 65535) */
#define WIFI_MAC_ADDR_STR_LEN             		18U
#define WIFI_CHANNEL_STR_LEN           			3U                         /* 2 characters + NULL (1 to 14) */
#define WIFI_LIST_SIZE                          10U
#define INVALID_NET_ID                          0xFF
#define WIFI_SCAN_COMMAND                       "SCAN"
#define WIFI_SCAN_RESULTS_COMMAND               "SCAN_RESULTS"

typedef enum
{
   WIFI_SECURITY_INVALID=0,
   WIFI_SECURITY_OPEN,
   WIFI_SECURITY_WEP,
   WIFI_SECURITY_WPA,
   WIFI_SECURITY_WPA2
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
   WiFiSecurityType_t SecurityType;
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



wifi_network_list_t g_wifi_scan_list;

struct wpa_ctrl *p_wpa_ctrl_send;
struct wpa_ctrl *p_wpa_ctrl_receive;

void CloseWiFi(void);

static char CheckWiFiServiceStarted(void);
static void wiFiSendCommand(char *p_command);
static void parseWiFiScanResults(char *p_results);
static void getWiFiNetworkSecurityType(char *ptr);
static int ExtractString(char *p_in, char *p_out, char stop, short length);
static void clearWifiNetworkList(wifi_network_list_t *p_netList);
static void clearWifiNetwork(WiFiNetwork_t *p_network);

/*****************************************************************************
 *
 */
char InitWiFi(void)
{
	char return_val = 0;

	//first check if the service is running
	if(CheckWiFiServiceStarted())
	{
#if 0
		p_wpa_ctrl_send = wpa_ctrl_open("/var/run/wpa_supplicant/wlan0");
		if (p_wpa_ctrl_send == 0)
		{
			printf("Wifi not available: p_wpa_ctrl_send = 0\nNo wpa_supplicant at /var/run/\n");
			CloseWiFi();
		}
		else
		{
			p_wpa_ctrl_receive = wpa_ctrl_open("/var/run/wpa_supplicant/wlan0");

			// make sure it opened successfully
			if (p_wpa_ctrl_receive != 0)
			{
				// try to attach so that we can receive messages
				if (wpa_ctrl_attach(p_wpa_ctrl_receive) >= 0)
				{
					//printf("WiFi Control Attached\n");
					return_val = 1;
				}
				else
				{
					printf("Wifi not available: wpa_ctrl_attach < 0\n");
					CloseWiFi();
				}
			}
			else
			{
				printf("Wifi not available: p_wpa_ctrl_receive = 0\n");
				CloseWiFi();
			}
		}
#endif
	}
	else
	{
		printf("WiFi Service not running\n");
		CloseWiFi();
	}

	return return_val;
}

/*****************************************************************************
 *
 */
void DoWiFiScan(void)
{
	printf("Doing WiFi Scan\n");

	wiFiSendCommand(WIFI_SCAN_COMMAND);

	wiFiSendCommand(WIFI_SCAN_RESULTS_COMMAND);
}

/*****************************************************************************
 *
 */
void CloseWiFi(void)
{
#if 0
	if(p_wpa_ctrl_send != 0)
	{
		wpa_ctrl_close(p_wpa_ctrl_send);
	}

	if(p_wpa_ctrl_receive != 0)
	{
		wpa_ctrl_detach(p_wpa_ctrl_receive);
		wpa_ctrl_close(p_wpa_ctrl_receive);
	}
#endif
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
 *      - TRUE is initialized, FALSE otherwise
 *
 * Notes:
 *
 *******************************************************************************************************************************/
static char CheckWiFiServiceStarted(void)
{
    FILE *fp;
    char command[100];
    char buf[100];
    char retVal = 0;

    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "systemctl status wpa_supplicant-nl80211@wlan0 | grep 'active (running)'");

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
                retVal = 1;
                break;
            }
            else
            {
            	printf("wpa_supplicant-nl80211@wlan0 is not running\n");
            }
        }

        if (pclose(fp) == -1)
        {
            printf("Error closing pipe!\n");
        }
    }

    return retVal;
}

/*******************************************************************************************************************************
 * static WiFiControlStates_t wiFiSendCommand(char *command)
 *
 *  send a command to the WiFi module through the wpa_supplicant control interface
 *
 * Parameters:
 *      - command:
 *
 * Returns:
 *      - the next state for the WiFi state machine
 *
 * Notes:
 *
 *******************************************************************************************************************************/
static void wiFiSendCommand(char *p_command)
{
    char response[4096];
    size_t response_len = sizeof(response);

    memset(response, 0, response_len);
#if 0
    wpa_ctrl_request(p_wpa_ctrl_send, p_command, strlen(p_command), response, &response_len, 0);
#endif
    // save the scan results
	if (!strncmp(p_command, WIFI_SCAN_RESULTS_COMMAND, strlen(WIFI_SCAN_RESULTS_COMMAND)))
	{
		parseWiFiScanResults(response);
	}
}

/*******************************************************************************************************************************
 * static void parseWiFiScanResults(char *results)
 *
 *  parse through the scan results that were sent to us by wpa_supplicant and store the relevant data in the scan
 *  list structure
 *
 * Parameters:
 *      - results: results passed to use by wpa_supplicant
 *
 * Returns:
 *      - none
 *
 * Notes:
 *
 *******************************************************************************************************************************/
static void parseWiFiScanResults(char *p_results)
{
    char *ptr;
    char signalStrength[5];
    int length = 0;

    clearWifiNetworkList((wifi_network_list_t *)&g_wifi_scan_list);
    g_wifi_scan_list.length = 0;

    ptr = p_results;

    // get past the first line which doesn't have any useful info yet
    length = ExtractString(ptr, 0, '\n', 0);
    ptr += (length + 1);

    // go through the scan results and store the useful info in the scan list
    while (*ptr != '\0')
    {
        if (g_wifi_scan_list.length < WIFI_LIST_SIZE)
        {
            // the different sections are separated by tabs - the first section is the MAC address
            length = ExtractString(ptr, (char *)&g_wifi_scan_list.network[g_wifi_scan_list.length],
                                   '\t', WIFI_MAC_ADDR_STR_LEN);

            // we don't care about the next section
            ptr += (length + 1);
            length = ExtractString(ptr, 0, '\t', 0);

            // add length +2 so that we skip over the negative sign - storing signal strength as unsigned number
            ptr += (length + 2);

            // store the signal strength
            memset(signalStrength, 0, sizeof(signalStrength));
            length = ExtractString(ptr, signalStrength, '\t', sizeof(signalStrength));
            g_wifi_scan_list.network[g_wifi_scan_list.length].Power = atoi(signalStrength);

            ptr += (length + 1);

            // store the security type
            getWiFiNetworkSecurityType(ptr);

            // store the SSID
            length = ExtractString(ptr, 0, '\t', 0);
            ptr += (length + 1);
            length = ExtractString(ptr, (char *)g_wifi_scan_list.network[g_wifi_scan_list.length].Header.SSIDName,
                                   '\n', WIFI_SSID_STR_LEN);
            ptr += (length + 1);

            g_wifi_scan_list.length++;
        }
        else
        {
            break;
        }
    }

    printf("WiFi Scan Complete\n");

	printf("\n");

	// print the results
	int i = 0;
	for (i = 0; i < g_wifi_scan_list.length; i++)
	{
		printf("%s\t[%d]", g_wifi_scan_list.network[i].Header.SSIDName, g_wifi_scan_list.network[i].Power);

		switch (g_wifi_scan_list.network[i].Header.SecurityType)
		{
		case WIFI_SECURITY_WPA2:
			printf("\t[WPA2]\n");
			break;
		case WIFI_SECURITY_WPA:
			printf("\t[WPA]\n");
			break;
		case WIFI_SECURITY_WEP:
			printf("\t[WEP]\n");
			break;
		case WIFI_SECURITY_OPEN:
			printf("\t[OPEN]\n");
			break;
		case WIFI_SECURITY_INVALID:
		default:
			break;
		}
	}

}


/*******************************************************************************************************************************
 * static void getWiFiNetworkSecurityType(char *ptr)
 *
 *  store the security type for the current network in the scan list
 *
 * Parameters:
 *      - ptr:
 *
 * Returns:
 *      - none
 *
 * Notes:
 *
 *******************************************************************************************************************************/
static void getWiFiNetworkSecurityType(char *ptr)
{
    char securityData[100];

    memset(securityData, 0, sizeof(securityData));
    ExtractString(ptr, securityData, '\t', sizeof(securityData));

    if (strstr(securityData, "WPA2-PSK"))
    {
        g_wifi_scan_list.network[g_wifi_scan_list.length].Header.SecurityType = WIFI_SECURITY_WPA2;
    }
    else if (strstr(securityData, "WPA-PSK"))
    {
        g_wifi_scan_list.network[g_wifi_scan_list.length].Header.SecurityType = WIFI_SECURITY_WPA;
    }
    else if (strstr(securityData, "WEP"))
    {
        g_wifi_scan_list.network[g_wifi_scan_list.length].Header.SecurityType = WIFI_SECURITY_WEP;
    }
    else
    {
        g_wifi_scan_list.network[g_wifi_scan_list.length].Header.SecurityType = WIFI_SECURITY_OPEN;
    }
}

/*******************************************************************************************************************************
 * void ExtractString(char *in, char *out, char stop, U16 length)
 *
 *  Certain responses that we receive have information that we need inside of them. This method is used to get rid of the
 *  rest of the command and just return the useful information that we need
 *
 * Parameters:
 *      -in: the command that contains the information inside it
 *      -out: the buffer to place the output information
 *      -stop: the char that signals the end of the string that we're looking for
 *      -length of the output buffer
 *
 * Returns:
 *      - the length of the output string
 *
 * Notes:
 *      - you can pass in NULL for the output string in order to get the amount of characters between the start of the
 *        string and the stop character
 *
 ******************************************************************************************************************************/
static int ExtractString(char *p_in, char *p_out, char stop, short length)
{
    short in_index = 0;
    short out_index = 0;

    while (p_in[in_index] != stop)
    {
        if (p_out != 0)
        {
            if (in_index < length)
            {
                p_out[out_index++] = p_in[in_index++];
            }
            else
            {
                // make sure there is a null terminator
                p_out[out_index] = 0;
                break;
            }
        }
        else
        {
            // just in case...
            if (in_index > 200)
                break;

            in_index++;
            out_index++;
        }
    }

    return out_index;
}


/*******************************************************************************************************************************
 * void clearWifiNetworkList(wifi_network_list_t *p_netList)
 *
 * clear a WiFinetwork list structure
 *
 * Parameters:
 *      - *p_netList: pointer to the list that needs to be cleared
 *
 * Returns:
 *      - none
 *
 * Notes:
 *
 *******************************************************************************************************************************/
static void clearWifiNetworkList(wifi_network_list_t *p_netList)
{
    int i;

    for (i = 0; i < WIFI_LIST_SIZE; i++)
    {
        clearWifiNetwork((WiFiNetwork_t *)&p_netList->network[i]);
    }

    p_netList->length = 0;
}

/*******************************************************************************************************************************
 * void clearWifiNetwork(WiFiNetwork_t *p_network)
 *
 *  clears the specified WiFinetwork
 *
 * Parameters:
 *      - p_network: the network to clear
 *
 * Returns:
 *      - none
 *
 * Notes:
 *      This function is necessary because the net_id of WiFiNetwork_t structures should default to INVALID_NET_ID (0xFF),
 *      and not 0
 *
 *******************************************************************************************************************************/
static void clearWifiNetwork(WiFiNetwork_t *p_network)
{
    if (p_network != 0)
    {
        memset(p_network, 0, sizeof(WiFiNetwork_t));
        p_network->Header.net_id = INVALID_NET_ID;
        p_network->Index = -1;
    }
}
