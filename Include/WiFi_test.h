/*
 * WiFi_test.h
 *
 *  Created on: Oct 28, 2025
 *      Author: michaeljacobson
 */

#ifndef INCLUDE_WIFI_TEST_H_
#define INCLUDE_WIFI_TEST_H_

#define INVALID_INDEX -1

typedef enum
{
	WIFI_IDLE = 0,
	WIFI_SCAN,
	WIFI_JOIN,
	WIFI_STATUS,
	WIFI_LEAVE,
	WIFI_DELETE
} WiFiStates_t;

char InitWiFi(void);
void CloseWiFi(void);
void SetPasswordForWiFiNetwork(int index, char *p_password);
int ChangeWiFiState(WiFiStates_t newState, int index);

#endif /* INCLUDE_WIFI_TEST_H_ */
