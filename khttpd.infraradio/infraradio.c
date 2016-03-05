#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <sys/un.h>

#include <glib.h>
#include <microhttpd.h>
#include <json/json.h>
#include <wiringPi.h>

#include "kohaerenzstiftung.h"
#include "httpd.h"



static uint32_t transmitterPin = -1;
static char *path2Lircd = NULL;
static char *path2LircdConf = NULL;
//curl -u sancho:sancho  http://raspberry2:8080/infraradio
//curl -X PUT -u sancho:sancho -H "bitmapString: 00000"  -H "type: radio" -H "socketNumber: 1" -H "onOff: 0"  http://raspberry2:8080/infraradio

FUNCTION(teardown, void, PRIVATE, (void),
	free(path2LircdConf);
	free(path2Lircd);
)

FUNCTION(setup, void, PRIVATE, (GList *keyValuePairs, err_t *e),
	char *lircdPath = NULL;
	char *transmitterPinString = NULL;
	char *lircdConfPath = NULL;

	terror(lircdConfPath = getValue(keyValuePairs, "lircdConfPath", e))

	terror(lircdPath = getValue(keyValuePairs, "path2Lircd", e))
	terror(transmitterPinString = getValue(keyValuePairs, "transmitterPin", e))

	terror(transmitterPin = parseInteger(transmitterPinString, FALSE, e))
	path2Lircd = strdup(lircdPath);
	path2LircdConf = strdup(lircdConfPath);

	terror(failIfFalse((wiringPiSetup() == 0)))
	pinMode(transmitterPin, OUTPUT);

finish:
	return;
)

FUNCTION(connect2Lircd, int, PRIVATE, (char *path2Lircd, err_t *e),
	int result = -1;
	int fd = -1;
	uint32_t max = 0;
	struct sockaddr_un sockaddr_un;

	sockaddr_un.sun_family = AF_UNIX;
	strncpy(sockaddr_un.sun_path, path2Lircd, (max = (sizeof(sockaddr_un.sun_path) - 1)));
	sockaddr_un.sun_path[max] = '\0';

	terror(failIfFalse(((fd = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0)))
	terror(failIfFalse(((connect(fd, &sockaddr_un, sizeof(sockaddr_un))) >= 0)))
	result = fd; fd = -1;

finish:
	if (fd >= 0) {
		close(fd);
	}
	return result;
)

FUNCTION(getReply, void, PRIVATE, (int fd, err_t *e),
	int selectResult = -1;
	fd_set readfds;
	char buffer[10];
	struct timeval timeval;
	char SUCCESS[] = "SUCCESS";
	size_t length = sizeof(SUCCESS) - 1;

	timeval.tv_sec = 0;
	timeval.tv_usec = 100000;

	for(;;) {
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);

		selectResult = select((fd + 1), &readfds, NULL, NULL, &timeval);
		terror(failIfFalse((selectResult >= 0)))
		if (selectResult < 1) {
			break;
		}
		terror(failIfFalse((read(fd, buffer, sizeof(buffer)) > 0)))
		if (strncmp(SUCCESS, buffer, length) == 0) {
			goto finish;
		}
	}

	terror(failIfFalse(FALSE))

finish:
	return;
)

FUNCTION(putInfrared, void, PRIVATE, (GList *requestHeaders, err_t *e),
	//int status = 0;
	char *device = NULL;
	char *key = NULL;
	int fd = -1;
	char *request = NULL;

	terror(device = getValue(requestHeaders, "device", e))
	terror(key = getValue(requestHeaders, "key", e))

	terror(fd = connect2Lircd(path2Lircd, e))
	asprintf(&request, "SEND_ONCE %s %s\n", device, key);
	terror(writeBytes(fd, request, strlen(request), e))
	terror(getReply(fd, e))

finish:
	if (fd >= 0) {
		close(fd);
	}
	free(request);
)

const char* code[6] = { "FFFFF", "0FFFF", "F0FFF", "FF0FF", "FFF0F", "FFFF0" };

FUNCTION(getCodeWord, char *, PRIVATE, (char *systemCode, int unitCode, gboolean on, err_t *e),
	int nReturnPos = 0;
	char res[13];
	char *result = NULL;
	int i = -1;

	if (unitCode < 1 || unitCode > 5) {
		result = strdup("");
		goto finish;
	}

	for (i = 0; i < 5; i++) {
		if (systemCode[i] == '0') {
			res[nReturnPos++] = 'F';
		} else if (systemCode[i] == '1') {
			res[nReturnPos++] = '0';
		} else {
			result = strdup("");
			goto finish;
		}
	}

	for (i = 0; i < 5; i++) {
		res[nReturnPos++] = code[ unitCode ][i];
	}

	if (on) {
		res[nReturnPos++] = '0';
		res[nReturnPos++] = 'F';
	} else {
		res[nReturnPos++] = 'F';
		res[nReturnPos++] = '0';
	}

	res[nReturnPos] = '\0';
	result = strdup(res);

finish:
	return result;
)

FUNCTION(transmit, void, PRIVATE, (int nHighPulses, int nLowPulses),

	digitalWrite(transmitterPin, HIGH);
#define PULSE_LENGTH 350
	delayMicroseconds(PULSE_LENGTH * nHighPulses);
	digitalWrite(transmitterPin, LOW);
	delayMicroseconds(PULSE_LENGTH * nLowPulses);

)

FUNCTION(sendT0, void, PRIVATE, (),
	transmit(1, 3);
	transmit(1, 3);
)

FUNCTION(sendTF, void, PRIVATE, (),
	transmit(1, 3);
	transmit(3, 1);
)

FUNCTION(sendT1, void, PRIVATE, (),
	transmit(3, 1);
	transmit(3, 1);
)

FUNCTION(sendSync, void, PRIVATE, (),
	transmit(1, 31);
)

FUNCTION(sendCodeword, void, PRIVATE, (char *codeWord, err_t *e),
	int nRepeat = -1;
	int i = -1;

#define REPEAT_TRANSMIT 10
	for (nRepeat = 0; nRepeat < REPEAT_TRANSMIT; nRepeat++) {
		i = 0;
		while (codeWord[i] != '\0') {
			switch(codeWord[i]) {
				case '0':
					sendT0();
				break;
				case 'F':
					sendTF();
				break;
				case '1':
					sendT1();
				break;
			}
			i++;
		}
		sendSync();
	}
)

FUNCTION(turnSwitch, void, PRIVATE, (gboolean on, char *systemCode, int unitCode, err_t *e),
	char *codeWord = NULL;

	terror(codeWord = getCodeWord(systemCode, unitCode, on, e))
	terror(sendCodeword(codeWord, e))

finish:
	free(codeWord);
)

FUNCTION(putRadio, void, PRIVATE, (GList *requestHeaders, err_t *e),
	char *systemCode = NULL;
	char *socketNumber = NULL;
	int unitCode = -1;
	char *onOff = NULL;
	gboolean on = FALSE;

	terror(systemCode = getValue(requestHeaders, "bitmapString", e))
	terror(socketNumber = getValue(requestHeaders, "socketNumber", e))
	terror(failIfFalse((socketNumber[0] != '\0')))
	terror(unitCode = parseInteger(socketNumber, FALSE, e))
	terror(onOff = getValue(requestHeaders, "onOff", e))
	if (strcmp(onOff, "0") != 0) {
		failIfFalse((strcmp(onOff, "1") == 0))
		on = TRUE;
	}

	terror(turnSwitch(on, systemCode, unitCode, e))

finish:
	return;
)

FUNCTION(put, void, PRIVATE, (char *url, int fromParent, int toParent, GList *requestHeaders, GList *requestParameters, gboolean *responseQueued, err_t *e),
	char *type = NULL;

	terror(type = getValue(requestHeaders, "type", e))
	if (strcmp(type, "infrared") == 0) {
		terror(putInfrared(requestHeaders, e))
	} else {
		terror(failIfFalse(strcmp(type, "radio") == 0))
		terror(putRadio(requestHeaders, e))
	}

finish:
	return;
)

FUNCTION(lineStartsWith, gboolean, PRIVATE, (char *line, char *startsWith[]),
	gboolean result = TRUE;
	int lineIdx = 0;
	int stringsIdx = 0;
	gboolean skipSpaces = TRUE;
	char *start = NULL;
	char restore = '\0';

	for(;;) {
		if (startsWith[stringsIdx] == NULL) {
			break;
		}
		if ((skipSpaces)&&(line[lineIdx] == '\0')) {
			result = FALSE;
			break;
		}

		if (skipSpaces) {
			if (!isspace(line[lineIdx])) {
				skipSpaces = FALSE;
				start = &line[lineIdx];
			}
		} else if (isspace(line[lineIdx])||(line[lineIdx] == '\0')) {
			restore = line[lineIdx]; line[lineIdx] = '\0';
			result = (strcmp(start, startsWith[stringsIdx]) == 0);
			line[lineIdx] = restore;
			if (!result) {
				break;
			}
			stringsIdx++;
			skipSpaces = TRUE;
		}
		lineIdx++;
	}
	return result;
)

FUNCTION(getFirstWord, char *, PRIVATE, (char *line, err_t *e),
	int i = 0;
	char *result = NULL;

	g_strstrip(line);
	terror(failIfFalse((line[0] != '\0')))

	for (i = 0; ((!isspace(line[i]))&&(line[i] != '\0')); i++);

	line[i] = '\0';
	result = line;

finish:
	return result;
)

FUNCTION(processCodes, struct json_object *, PRIVATE, (FILE *file, err_t *e),
	gboolean ok = FALSE;
	char line[500];
	char *nl = NULL;
	struct json_object *result = NULL;
	struct json_object *_result = json_object_new_array();
	char *name = NULL;

	for(;(fgets(line, sizeof(line), file) != NULL);) {
		nl = strchr(line, '\n');
		if (nl != NULL) {
			*nl = '\0';
		}
		g_strstrip(line);
		if (lineStartsWith(line, (char *[]) {"end", "codes", NULL})) {
			ok = TRUE;
			break;
		}

		if ((line[0] == '\0')||(line[0] == '#')) {
			continue;
		}

		terror(name = getFirstWord(line, e))
		json_object_array_add(_result, json_object_new_string(name));
	}

	terror(failIfFalse(ok))
	result = _result; _result = NULL;

finish:
	json_object_put(_result);
	return result;
)

FUNCTION(processRemote, struct json_object *, PRIVATE, (FILE *file, err_t *e),
	struct json_object *result = NULL;
	char line[500];
	gboolean ok = FALSE;
	gboolean haveName = FALSE;
	struct json_object *codes = NULL;
	char *name = NULL;
	char *nl = NULL;
	char stringName[] = "name";
	char *firstWord = NULL;
	int lengthName = sizeof(stringName) - 1;
	char *nameStart = NULL;

	for(;(!ok&&(fgets(line, sizeof(line), file) != NULL));) {

		nl = strchr(line, '\n');
		if (nl != NULL) {
			*nl = '\0';
		}
		if (!haveName) {
			if (!lineStartsWith(line, (char *[]) {stringName, NULL})) {
				continue;
			}
			nameStart = strstr(line, stringName);
			terror(failIfFalse((nameStart != NULL)))
			nameStart += lengthName;
			terror(firstWord = getFirstWord(nameStart, e))
			name = strdup(firstWord);
			haveName = TRUE;
		} else {
			if (!lineStartsWith(line, (char *[]) {"begin", "codes", NULL})) {
				continue;
			}
			terror(codes = processCodes(file, e))
			ok = TRUE;
		}
	}

	terror(failIfFalse(ok))

	result = json_object_new_object();
	json_object_object_add(result, "name", json_object_new_string(name));
	json_object_object_add(result, "codes", codes); codes = NULL;

finish:
	free(name);
	json_object_put(codes);
	return result;
)

FUNCTION(get, void, PRIVATE, (char *url, int fromParent, int toParent, GList *requestHeaders, GList *requestParameters, gboolean *responseQueued, err_t *e),
	const char *buffer = NULL;
	FILE *file = NULL;
	char line[500];
	char *nl = NULL;
	struct json_object *remote = NULL;
	struct json_object *remotes = json_object_new_array();

	terror(failIfFalse(((file = fopen(path2LircdConf, "r")) != NULL)))

	for(;(fgets(line, sizeof(line), file) != NULL);) {
		nl = strchr(line, '\n');
		if (nl != NULL) {
			*nl = '\0';
		}
		if (!lineStartsWith(line, (char *[]) {"begin", "remote", NULL})) {
			continue;
		}
		terror(remote = processRemote(file, e))
		json_object_array_add(remotes, remote); remote = NULL;
	}
	buffer = json_object_to_json_string(remotes);

	terror(respondFromBuffer(toParent, MHD_HTTP_OK,
		(void *) buffer, strlen(buffer), responseQueued, e))

finish:
	if (file != NULL) {
		fclose(file);
	}
	json_object_put(remote);
	json_object_put(remotes);
)

FUNCTION(startRegistry, void, PUBLIC, (err_t *e),
	terror(registerPlugin("infraradio", setup, teardown, put, NULL,
			get, NULL, NULL, e))
finish:
	return;
)
