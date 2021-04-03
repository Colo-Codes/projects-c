#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>

// ///// Definitions /////

#define PORT 161 //5000 // The port to listen for connections on
#define BACKLOG 10
#define MAXLINE 2048 // This should be the maximum message length possible (change if necessary)
#define SNMP_VER_LOC 4
#define SNMP_COMM_LOC 6
#define SNMP_COMM_MAX_LEN 256
#define ERROR_FIELD 3
#define END_OF_MIB_TREE 999
#define ERROR_NO_SUCH_OBJECT 0x80
#define ERROR_NO_SUCH_INSTANCE 0x81
#define ERROR_RESPONSE_END_OF_MIB_VIEW 0x82

#define GET_REQUEST 0xa0
#define GET_NEXT_REQUEST 0xa1
#define GET_RESPONSE 0xa2
#define SET_REQUEST 0xa3
#define GET_BULK_REQUEST 0xa5
#define INFORM_REQUEST 0xa6
#define TRAP 0xa7
#define REPORT 0xa8

#define SNMP_BER_SEQUENCE 0x30
#define SNMP_BER_INTEGER 0x02
#define SNMP_BER_OCTET_STRING 0x04
#define SNMP_BER_OBJECT_IDENTIFIER 0x06
#define SNMP_BER_NULL 0x05
#define SNMP_BER_IP_ADDRESS 0x40
#define SNMP_BER_COUNTER 0x41
#define SNMP_BER_GAUGE 0x42
#define SNMP_BER_TIME_TICKS 0x43
#define SNMP_BER_OPAQUE 0x44

#define SNMP_ERROR_TYPE_NO_ERROR 0x00
#define SNMP_ERROR_TYPE_TOO_BIG 0x01
#define SNMP_ERROR_TYPE_NO_SUCH_NAME 0x02
#define SNMP_ERROR_TYPE_BAD_VALUE 0x03
#define SNMP_ERROR_TYPE_READ_ONLY 0x04
#define SNMP_ERROR_TYPE_GEN_ERROR 0x05

#define LEN_ARRAY 4
#define LEN_SMALL 128
#define LEN_MEDIUM 256
#define OID_DATABASE_BATCH 256
#define OID_BATCH 32
#define OID_LEN 256
#define VARBINDS_BATCH 128
#define VALUE_LEN 512

#define SMALL_LEN 16
#define MEDIUM_LEN 32
#define LONG_LEN 64

// ///// Global variables /////

extern int errno;

int i;
int arrayIndex = 0; // needs to be able to be negative
unsigned int snmpPduLoc;
unsigned int reqIdLen;
unsigned int oidListIndex;
unsigned int oidListTotal;
unsigned int snmpPduType;
unsigned int snmpUpsDatabase_totalData = 0;
unsigned int snmpOidMibDatabase_totalData = 0;
unsigned int arrOfIntLenIndex = 0;
unsigned int calcLenLen = 0;
unsigned int oidConvertedLen = 0;
unsigned int flagErrorReadingFile = 0;
unsigned int flagErrorReadingDatabaseFile = 0;
unsigned int flagErrorReadingOidListFile = 0;
unsigned int oidLen = 0;
unsigned int getBulk_NonRepeaters = 0;
unsigned int getBulk_MaxRepetitions = 0;
unsigned int flag_oidFoundUpsDatabase = 0;
unsigned int errorType = 0x00;
unsigned int errorStatus = 0x00;
unsigned int errorIndex = 0x00;
unsigned int oid_value_hex_elementLen = 0;
unsigned int bufferLen = 0;
unsigned int bufferLenAux = 0;
unsigned int displacement = 0;
unsigned int bufferPreviousLen = 0;

unsigned char responseBuffer[MAXLINE];
unsigned char decodedOid[OID_LEN];
unsigned char oidString[OID_LEN];
unsigned char oidList[OID_BATCH][OID_LEN]; // Up to 32 OIDs requested at once
unsigned char reqIdVal[LEN_ARRAY];
unsigned char oidConvertedVal[LEN_ARRAY];
unsigned char configDataIp[16];
unsigned char configDataPort[10];
unsigned char configDataCommunity[LEN_SMALL];
unsigned char configDataSourcePort[10];
unsigned char snmpUpsDatabase_oid[OID_DATABASE_BATCH][OID_LEN];
unsigned char snmpUpsDatabase_value[OID_DATABASE_BATCH][LEN_MEDIUM];
unsigned char snmpOidMibDatabase_oid[OID_DATABASE_BATCH][OID_LEN];
unsigned char snmpOidMibDatabase_type[OID_DATABASE_BATCH][LEN_ARRAY];
unsigned char snmpOidMibDatabase_readOption[OID_DATABASE_BATCH][16];
unsigned char oidVal[OID_LEN];
unsigned char *arrOfLen[10];
unsigned char *arrOfIntLen[10];

struct varbinds
{
    //                  Varbind
    // |----------------------------------------|
    // Type Len            Value
    //             |----------------------------|
    //                  OID            Value
    //             |------------|  |------------|
    //             Type Len Value  Type Len Value

    unsigned int    index;
    // Varbind length
    unsigned int varbindVarbindLen[VARBINDS_BATCH];
    // Cumulative length
    unsigned int    varbindCumulativeLen[VARBINDS_BATCH];
    unsigned char   varbindCumulativeLen_hex[VARBINDS_BATCH][LEN_ARRAY];
    unsigned int    varbindCumulativeLen_hex_len[VARBINDS_BATCH];
    // OID from input
    unsigned char   oid_input[VARBINDS_BATCH][OID_LEN];
    // OID
    unsigned char   oid_type_hex[VARBINDS_BATCH][1];
    unsigned char   oid_len_hex[VARBINDS_BATCH][LEN_ARRAY];
    unsigned int    oid_len_hex_len[VARBINDS_BATCH];
    unsigned char   oid_value[VARBINDS_BATCH][OID_LEN];
    unsigned char   oid_value_hex[VARBINDS_BATCH][OID_LEN];
    unsigned int    oid_value_hex_element[VARBINDS_BATCH];
    unsigned int    oid_value_hex_elementLen[VARBINDS_BATCH];
    // Value
    unsigned int    value_type[VARBINDS_BATCH];
    unsigned int    value_value[VARBINDS_BATCH];
    unsigned char   value_value_hex[VARBINDS_BATCH][VALUE_LEN];
    unsigned int    value_value_hex_len[VARBINDS_BATCH];
    unsigned char   value_value_hex_len_hex[VARBINDS_BATCH][LEN_ARRAY];
    unsigned int    value_value_hex_len_hex_len[VARBINDS_BATCH];
};

struct varbinds varbindList;

// ///// Function declarations /////

void decodeRequest(unsigned char buffer[MAXLINE]);
void oidDecoder(unsigned int oidLen);
void getResponse();
void longIntCalculator(int longInt);
void longLenCalculator(int lenToCalc);
void bigOidConverter(unsigned int originalOid);
void readConfigData();
void fetchOidDatabaseValue();
void fetchOidListValue();
void oidParser(char oid[OID_LEN]);
void buildFinalBuffer();
void buildTLV(unsigned int indexTLV);
void buildVarbind_Oid(unsigned char oidListInput[OID_LEN]);
void findMatchingOid(unsigned char oidListInput[OID_LEN]);

// ///// Functions /////

int main()
{
    unsigned char buffer[MAXLINE]; // It is important to make it unsigned. Otherwise, bytes may contain extra values related to its sign.

    int sockfd; // Socket file descriptor
    struct sockaddr_in addr_local;
    struct sockaddr_in cliaddr; //, servaddr

    // === 1. Create a socket ===
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("ERROR: cannot create a socket\n");
        return (0);
    }
    else
    {
        printf("OK: successfully created a socket\n");
    }

    // === 1.1. Setup the local socket address ===
    addr_local.sin_family = AF_INET;         // Protocol Family
    addr_local.sin_port = htons(PORT);       // Port number
    addr_local.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
    bzero(&(addr_local.sin_zero), 8);        // Flush the rest of struct

    // === 1.2. Bind the socket ===
    if (bind(sockfd, (struct sockaddr *)&addr_local, sizeof(struct sockaddr)) == -1)
    {
        printf("ERROR: cannot bind Port %d\n", PORT);
        return (0);
    }
    else
    {
        printf("OK: bound to port %d successfully\n", PORT);
    }

    unsigned int len, n, snmpMsgSize, snmpMsgStart, snmpCommunityLen;

    len = sizeof(cliaddr); //len is value/result

    // === 2. Receive message ===
    // Keep listening and responding indefinitely
    while (1)
    {
        // Reset variables *** Important ***
        oid_value_hex_elementLen = 0;
        flag_oidFoundUpsDatabase = 0;
        errorType = 0x00;
        errorStatus = 0x00;
        errorIndex = 0x00;
        oidLen = 0;
        getBulk_NonRepeaters = 0;
        getBulk_MaxRepetitions = 0;
        snmpUpsDatabase_totalData = 0;
        snmpOidMibDatabase_totalData = 0;
        arrOfIntLenIndex = 0;
        calcLenLen = 0;
        oidConvertedLen = 0;
        flagErrorReadingFile = 0;
        flagErrorReadingDatabaseFile = 0;
        flagErrorReadingOidListFile = 0;
        arrayIndex = 0;
        varbindList.index = 0;
        bufferLen = 0;
        for (i = 0; i < VARBINDS_BATCH; i++)
        {
            varbindList.value_value_hex_len[i] = 0;
            varbindList.value_type[i] = 0;
            varbindList.varbindCumulativeLen_hex_len[i] = 0;
        }

        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        printf("\n (DEBUG) (MAIN) Message received >>> ");
        for (i = 0; i < sizeof(buffer); i++)
        {
            printf("%x ", buffer[i]);
        }
        printf("<<<");

        // === 3. Decode message ===
        // Find the first sequence type
        for (i = 0; i < MAXLINE; i++)
        {
            if (buffer[i] == 0x30)
            {
                unsigned int snmpMsgSeqAndLenExtra = 0;
                printf("\n (DEBUG) (MAIN) Start sequence type (0x30) found on position: %i", i);
                snmpMsgStart = i;
                // Checking the size of the SNMP message
                if (buffer[i + 1] == 0x81)
                {
                    snmpMsgSize = buffer[snmpMsgStart + 2] + buffer[snmpMsgStart + 3];
                    snmpMsgSeqAndLenExtra = 3;
                    snmpMsgStart = snmpMsgStart + snmpMsgSeqAndLenExtra - 1;
                }
                else if (buffer[i + 1] == 0x82)
                {
                    snmpMsgSize = buffer[snmpMsgStart + 2] * 256 + buffer[snmpMsgStart + 3];
                    snmpMsgSeqAndLenExtra = 4;
                    snmpMsgStart = snmpMsgStart + snmpMsgSeqAndLenExtra - 1;
                }
                else
                {
                    snmpMsgSize = buffer[i + 1];
                    snmpMsgSeqAndLenExtra = 3;
                    snmpMsgStart = snmpMsgStart + snmpMsgSeqAndLenExtra - 2;
                }
                printf("\n (DEBUG) (MAIN) The rest of the SNMP message size is: %i", snmpMsgSize);
                printf("\n (DEBUG) (MAIN) The total SNMP message size is: %i", snmpMsgSize + snmpMsgSeqAndLenExtra);

                // Break the for loop to prevent another "0x30" from messing up decoding process.
                break;
            }
        }

        // SNMP version
        if (buffer[snmpMsgStart + 3] == 1)
        {
            printf("\n (DEBUG) (MAIN) (OK) SNMP Version: v2c");
        }
        else
        {
            printf("\n (DEBUG) !!! ERROR !!! Invalid SNMP Version: start(%i): %x", snmpMsgStart + 3, buffer[snmpMsgStart + 3]);
            // exit(0);
        }

        // Get community length
        snmpCommunityLen = buffer[snmpMsgStart + 5];
        printf("\n (DEBUG) (MAIN) SNMP Community snmpCommunityLen: %i", snmpCommunityLen);
        unsigned char snmpCommunityStr[SNMP_COMM_MAX_LEN];
        for (i = 0; i < snmpCommunityLen; i++)
        {
            snmpCommunityStr[i] = buffer[snmpMsgStart + 5 + 1 + i];
            printf("\n (DEBUG) (MAIN) SNMP Community char %i: %c", i, snmpCommunityStr[i]);
        }
        printf("\n (DEBUG) (MAIN) SNMP Community: %s", snmpCommunityStr);

        // Check if community matches
        readConfigData();
        if (flagErrorReadingFile == 1)
        {
            printf("\n (DEBUG) (MAIN) !!! ERROR !!! Invalid community string: %s", snmpCommunityStr);
            // exit(0);
        }
        else
        {
            if (strcmp(snmpCommunityStr, configDataCommunity) == 0)
            {
                printf("\n (DEBUG (MAIN) (OK) SNMP Communities match");
            }
            else
            {
                printf("\n (DEBUG (MAIN) !!! ERROR !!! SNMP Communities does not match. Received SNMP Community: '%s' - Configured SNMP Community: '%s'", snmpCommunityStr, configDataCommunity);
                // exit(0);
            }
        }

        // As SNMP version and Community matches, get the sotred values on the server and the list of OID from MIBs
        fetchOidDatabaseValue();
        fetchOidListValue();

        // Get the PDU type
        snmpPduLoc = snmpMsgStart + snmpCommunityLen + 6;
        snmpPduType = buffer[snmpPduLoc]; // & 0xff; // Obtain last 2 chars. This is allowed because a char is represented as int.
        printf("\n (DEBUG (MAIN) SNMP PDU Type: %x", snmpPduType);

        // === 4. Choose response ===
        // Process rest of message
        if (snmpPduType == GET_REQUEST || snmpPduType == GET_NEXT_REQUEST || snmpPduType == GET_BULK_REQUEST)
        {
            printf("\n (DEBUG (MAIN) (OK) Request type %x received", snmpPduType);

            decodeRequest(buffer);

            getResponse();
        }
        else
        {
            printf("\n (DEBUG (MAIN) !!! ERROR !!! PDU type %x not recognised.", snmpPduType);
        }

        // Send the GetResponse message
        if (sendto(sockfd, responseBuffer, bufferLen, MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len))
        {
            printf("\n (DEBUG (MAIN) >>> GetResponse successfully sent.");
        }
        else
        {
            printf("\n (DEBUG (MAIN) >>> ERROR sending GetResponse: %d", errno);
        }
    }
}

void decodeRequest(unsigned char buffer[MAXLINE])
{
    unsigned int reqIdLenLoc = 0;
    unsigned int varBindListLenLoc = 0;
    unsigned int varBindListLenLocVal;
    unsigned int varBindX_LenLoc;
    unsigned int varBindX_OidLenLoc;
    unsigned int varBindX_OidLenLocVal;
    unsigned int varBindX_LenLocVal;
    unsigned int errorOrRepeaters = 0;
    // unsigned int reqIdInt;

    // Get Request ID
    if (buffer[snmpPduLoc + 1] == 0x81)
    {
        reqIdLenLoc = snmpPduLoc + 4;
    }
    else if (buffer[snmpPduLoc + 1] == 0x82)
    {
        reqIdLenLoc = snmpPduLoc + 5;
    }
    else
    {
        reqIdLenLoc = snmpPduLoc + 3;
    }

    reqIdLen = buffer[reqIdLenLoc];
    printf("\n (DEBUG (DECODE REQUEST) Request ID length: %i", reqIdLen);

    for (i = 0; i < reqIdLen; i++)
    {
        reqIdVal[i] = buffer[reqIdLenLoc + 1 + i];
        // printf("\n (DEBUG (DECODE REQUEST) Request ID char %i: %x", i, reqIdVal[i]);
    }

    // Error fields are ignored for requests (except for GetBulkRequest)
    if (snmpPduType == GET_BULK_REQUEST)
    {
        // Number of array elements coresponding to the NonRepeaters integer
        unsigned int nonRepeatersArrayLen = buffer[reqIdLenLoc + reqIdLen + 2];
        unsigned char nonRepeatersArray[3];
        for (i = 0; i < nonRepeatersArrayLen; i++)
        {
            nonRepeatersArray[i] = buffer[reqIdLenLoc + reqIdLen + 3 + i];
        }
        if (nonRepeatersArrayLen == 1)
        {
            getBulk_NonRepeaters = nonRepeatersArray[0];
        }
        else if (nonRepeatersArrayLen == 2)
        {
            getBulk_NonRepeaters = 256 * nonRepeatersArray[0] + nonRepeatersArray[1];
        }
        else
        {
            printf("\n (DEBUG (DECODE REQUEST) !!! ERROR !!! Too many OID requested. This operation is not supported.");
        }
        printf("\n (DEBUG (DECODE REQUEST) GetBulkRequest - getBulk_NonRepeaters: %i", getBulk_NonRepeaters);

        // Number of array elements corresponding to the MaxRepetitions integer
        unsigned int maxRepetitionsArrayLen = buffer[reqIdLenLoc + reqIdLen + 2 + nonRepeatersArrayLen + 2];
        unsigned char maxRepetitionsArray[3];
        for (i = 0; i < maxRepetitionsArrayLen; i++)
        {
            maxRepetitionsArray[i] = buffer[reqIdLenLoc + reqIdLen + 2 + nonRepeatersArrayLen + 3 + i];
        }
        if (maxRepetitionsArrayLen == 1)
        {
            getBulk_MaxRepetitions = maxRepetitionsArray[0];
        }
        else if (maxRepetitionsArrayLen == 2)
        {
            getBulk_MaxRepetitions = 256 * maxRepetitionsArray[0] + maxRepetitionsArray[1];
        }
        else
        {
            printf("\n (DEBUG (DECODE REQUEST) !!! ERROR !!! Too many OID requested. This operation is not supported.");
        }
        printf("\n (DEBUG (DECODE REQUEST) GetBulkRequest - getBulk_MaxRepetitions: %i", getBulk_MaxRepetitions);

        errorOrRepeaters = 2 + nonRepeatersArrayLen + 2 + maxRepetitionsArrayLen;
    }
    else
    {
        errorOrRepeaters = 2 * ERROR_FIELD;
    }

    // VarBinds
    if (buffer[snmpPduLoc + 1] == 0x81)
    {
        varBindListLenLoc = reqIdLenLoc + reqIdLen + errorOrRepeaters + 3;
    }
    else if (buffer[snmpPduLoc + 1] == 0x82)
    {
        varBindListLenLoc = reqIdLenLoc + reqIdLen + errorOrRepeaters + 4;
    }
    else
    {
        varBindListLenLoc = reqIdLenLoc + reqIdLen + errorOrRepeaters + 2;
    }

    printf("\n (DEBUG (DECODE REQUEST) varBindListLenLoc: %i", varBindListLenLoc);

    varBindListLenLocVal = buffer[varBindListLenLoc];

    // varbind 1
    varBindX_LenLoc = varBindListLenLoc + 2;
    varBindX_LenLocVal = buffer[varBindX_LenLoc];
    varBindX_OidLenLoc = varBindX_LenLoc + 2;
    varBindX_OidLenLocVal = buffer[varBindX_OidLenLoc];

    // Get the OID
    for (i = 0; i < varBindX_OidLenLocVal; i++)
    {
        decodedOid[i] = buffer[varBindX_OidLenLoc + 1 + i];
    }
    // Reset OID string
    strcpy(oidString, "");

    // Decode and print
    oidListIndex = 0;
    oidListTotal = 1;
    oidDecoder(varBindX_OidLenLocVal);
    strcpy(oidList[oidListIndex], oidString);
    printf("\n (DEBUG (DECODE REQUEST) >>> Decoded OID: %s", oidList[oidListIndex]);

    // Get the values (null for GetRequest)
    printf("\n (DEBUG (DECODE REQUEST) Is there another sequence on position (%i)?: %x (x)", varBindX_LenLoc + varBindX_LenLocVal + 1, buffer[varBindX_LenLoc + varBindX_LenLocVal + 1]);

    // Rest of varbinds
    while (buffer[varBindX_LenLoc + varBindX_LenLocVal + 1] == 0x30) // A varbind sequence
    {
        oidListIndex++;
        oidListTotal++;

        // Clean the OID buffer
        for (i = 0; i < sizeof(decodedOid); i++)
        {
            decodedOid[i] = '\0';
        }

        varBindX_LenLoc = varBindX_LenLoc + varBindX_LenLocVal + 2;
        varBindX_LenLocVal = buffer[varBindX_LenLoc];
        varBindX_OidLenLoc = varBindX_LenLoc + 2;
        varBindX_OidLenLocVal = buffer[varBindX_OidLenLoc];

        // Get the OID
        for (i = 0; i < varBindX_OidLenLocVal; i++)
        {
            decodedOid[i] = buffer[varBindX_OidLenLoc + 1 + i];
            // printf("\n (DEBUG (DECODE REQUEST) Decoded OID %i: %x (x)", i, decodedOid[i]);
        }

        // Reset OID string
        strcpy(oidString, "");
        // Decode and print
        oidDecoder(varBindX_OidLenLocVal);
        strcpy(oidList[oidListIndex], oidString);
        printf("\n (DEBUG (DECODE REQUEST) >>> Decoded OID: %s", oidList[oidListIndex]);

        // Get the values (null for GetRequest)
        printf("\n (DEBUG (DECODE REQUEST) Is there another sequence on position (%i)?: %x (x)", varBindX_LenLoc + varBindX_LenLocVal + 1, buffer[varBindX_LenLoc + varBindX_LenLocVal + 1]);
    }
}

void oidDecoder(unsigned int oidLenDecoder)
{
    unsigned int oidIntValue;
    unsigned char oidStringAux[16];

    if (decodedOid[0] == 0x2b)
    {
        strcat(oidString, ".1.3");
    }
    else
    {
        // This should never be triggered. This is a failsafe.
        sprintf(oidString, ".%i", decodedOid[0]);
    }

    for (i = 1; i < oidLenDecoder; i++)
    {
        // i = 0 is the .1.3 section of th OID

        // OID BER encoding method
        // -----------------------
        // If 1 byte: byte1
        //     Max value is 127 = x7f. Changes on 128 => x81
        // If 2 bytes: (( byte1 - x80 ) * x80 + byte2
        //     Max value is 16383 = xff x7f. Changes on 16384 => x81 x80
        // If 3 bytes: (( byte1 - x80 ) * x80 + (byte2 - x80)) * x80 + byte3
        // More than 3 bytes are not used in OIDs for SNMP
        if (decodedOid[i] <= 127)
        {
            // printf("\n (DEBUG (OID DECODER) +++ OID - single byte: %x", decodedOid[i]);
            // printf("\n (DEBUG (OID DECODER) +++ OID - int value:   %i", decodedOid[i]);
            sprintf(oidStringAux, ".%i", decodedOid[i]);
        }
        else if (decodedOid[i] > 127 && decodedOid[i + 1] <= 127)
        {
            // printf("\n (DEBUG (OID DECODER) +++ OID - double bytes: %x %x", decodedOid[i], decodedOid[i + 1]);
            oidIntValue = ((decodedOid[i] - 0x80) * 0x80) + decodedOid[i + 1];
            // printf("\n (DEBUG (OID DECODER) +++ OID - int value:   %i", oidIntValue);
            sprintf(oidStringAux, ".%i", oidIntValue);
            i++;
        }
        else if (decodedOid[i] > 127 && decodedOid[i + 1] > 127 && decodedOid[i + 2] <= 127)
        {
            // printf("\n (DEBUG (OID DECODER) +++ OID - triple bytes: %x %x %x", decodedOid[i], decodedOid[i + 1], decodedOid[i + 2]);
            oidIntValue = (((decodedOid[i] - 0x80) * 0x80) + (decodedOid[i + 1] - 0x80)) * 0x80 + decodedOid[i + 2];
            // printf("\n (DEBUG (OID DECODER) +++ OID - int value:   %i", oidIntValue);
            sprintf(oidStringAux, ".%i", oidIntValue);
            i += 2;
        }
        else
        {
            printf("\n (DEBUG (OID DECODER) +++ OID - ERROR: OID size too big and not supported.");
        }
        // Convert OIDs to string
        strcat(oidString, oidStringAux);
    }
}

void getResponse()
{
    unsigned int revIndex;
    unsigned int arrIndex;
    unsigned int snmpPduLenArrayLen = 0;
    unsigned int varbindTotalLenArrayLen = 0;
    unsigned int snmpCommLenArrayLen = 0;
    unsigned int snmpFullLenArrayLen = 0;

    unsigned char varbindTotalLenArray[LEN_ARRAY];
    unsigned char snmpPduLenArray[LEN_ARRAY];
    unsigned char snmpCommLenArray[LEN_ARRAY];
    unsigned char snmpFullLenArray[LEN_ARRAY];
    unsigned char temp[1][LEN_SMALL];

    // Invert OIDs order
    revIndex = 0;
    arrIndex = oidListTotal - 1;
    while (revIndex < arrIndex)
    {
        strcpy(temp[0], oidList[revIndex]);
        strcpy(oidList[revIndex], oidList[arrIndex]);
        strcpy(oidList[arrIndex], temp[0]);
        revIndex++;
        arrIndex--;
    }

    // === 1. OIDs and values ===
    // ... +-----+-----+-----+-----+-------+-----+-----+*****+*******+
    // ... | Seq | Len | OID | Value | ... | Seq | Len | OID | Value |
    // ... +-----+-----+-----+-----+-------+-----+-----+*****+*******+

    if (snmpPduType == GET_REQUEST || snmpPduType == GET_NEXT_REQUEST)
    {
        for (oidListIndex = 0; oidListIndex < oidListTotal; oidListIndex++)
        {
            buildVarbind_Oid(oidList[oidListIndex]);

            strcpy(varbindList.oid_input[varbindList.index], oidList[oidListIndex]);

            // Important:
            varbindList.index++;
        }
    }
    else if (snmpPduType == GET_BULK_REQUEST)
    {
        // ///// Processing Repeaters (MaxRepetitions) /////

        unsigned int numRepeatingOids = oidListTotal - getBulk_NonRepeaters;
        unsigned int totalOutOids = getBulk_MaxRepetitions * numRepeatingOids;
        unsigned int indexToOut;

        // For each repetition until the maximum of repetitions
        for (indexToOut = 0; indexToOut < totalOutOids; indexToOut++)
        {
            // The first elements in the repetitions are the GetNext of the requested OIDs
            if (indexToOut < numRepeatingOids)
            {
                buildVarbind_Oid(oidList[indexToOut]);
            }
            else
            {
                // The rest of the elements in the repetition are the output of the previous ones
                buildVarbind_Oid(varbindList.oid_value[indexToOut - numRepeatingOids]);
            }
            varbindList.index++;
        }

        // ///// Processing NonRepeaters /////

        unsigned int j;

        for (j = 0; j < getBulk_NonRepeaters; j++)
        {
            buildVarbind_Oid(oidList[oidListTotal - getBulk_NonRepeaters + j]);
            strcpy(varbindList.oid_input[varbindList.index], oidList[oidListTotal - getBulk_NonRepeaters + j]);

            oidListIndex++;

            // Important:
            varbindList.index++;
        }
    }

    // ///// Build the buffer to be transmitted /////

    buildFinalBuffer();
}

void longIntCalculator(int longInt)
{
    // LOGIC:
    // 1 - Obtain the long number.
    // 2 - Convert it to hex (to keep track of what needs to be sent).
    // 3 - Convert each hex to to int (in order to send the int, which will be interpreted as hex).
    // 4 - Save those int in and array (so they are actually transferred as hex).

    printf("\n (DEBUG (LONG INT CALCULATOR) >> longIntCalculator started...");
    if (longInt >= 2147483647) // 7f ff ff ff
    {
        printf("\n  (!!) Error: the used integer is too big (over 2,147,483,647).");
        // exit(-1);
    }
    else
    {
        free(arrOfIntLen);
        // memset(arrOfIntLen, '\0', strlen(arrOfIntLen));
        int i = 0;
        arrOfIntLenIndex = 0;
        unsigned char longIntArr[10];
        snprintf(longIntArr, sizeof(longIntArr), "%x", longInt);
        // printf("\n  (DEBUG (LONG INT CALCULATOR) longIntArr (x i): '%x' '%i' - longInt (x i): '%x' '%i'", longIntArr, longIntArr, longInt, longInt);

        // Calculate the amount of hexadecimal digits
        int hexLen = 0;
        // printf("\n  (DEBUG (LONG INT CALCULATOR) longInt (x i): '%x' '%i'", longInt, longInt);
        while (longInt != 0)
        {
            longInt = longInt / 16;
            // printf("\n  (DEBUG (LONG INT CALCULATOR) (looping to get hex len) longInt (x i): '%x' '%i'", longInt, longInt);
            hexLen++;
            // printf("\n  (DEBUG (LONG INT CALCULATOR) (looping to get hex len) hexLen (x i): '%x' '%i'", hexLen, hexLen, hexLen, hexLen);
        }

        // Get the hexadecimal values
        char longIntData[2];
        for (i = 0; i < hexLen; i++)
        {
            // printf("\n  (DEBUG (LONG INT CALCULATOR) i = '%i'", i);
            // printf("\n  (DEBUG (LONG INT CALCULATOR) longIntData (x i): '%x' '%i'", longIntData, longIntData);

            if (hexLen < 2 || (hexLen % 2 != 0 && i == 0)) // The only digit, or the first digit of odd number of digits, should be single hex digit
            {
                memcpy(longIntData, &longIntArr[i], 1); // Take one digit
            }
            else
            {
                memcpy(longIntData, &longIntArr[i], 2); // Take two digits
            }

            if (i % 2 == 0 && hexLen % 2 == 0) // For even number of hex digits
            {
                // printf("\n  (DEBUG (LONG INT CALCULATOR) longIntData (x c i s)   : '%x' '%c' '%i' '%s'", longIntData, longIntData, longIntData, longIntData);
                int number = (int)strtol(longIntData, NULL, 16);
                // printf("\n  (DEBUG (LONG INT CALCULATOR) Converted value (x c i s): '%x' '%c' '%i' '%s'", number, number, number, number);
                arrOfIntLen[arrOfIntLenIndex++] = number;
                // printf("\n  (DEBUG (LONG INT CALCULATOR) (arrOfIntLen): '%x' '%c' '%i' '%s'", arrOfIntLen[arrOfIntLenIndex-1], arrOfIntLen[arrOfIntLenIndex-1], arrOfIntLen[arrOfIntLenIndex-1], arrOfIntLen[arrOfIntLenIndex-1]);
            }
            else if ((i == 0 || i % 2 != 0) && hexLen % 2 != 0)
            {
                // printf("\n  (DEBUG (LONG INT CALCULATOR) longIntData (x c i s)   : '%x' '%c' '%i' '%s'", longIntData, longIntData, longIntData, longIntData);
                int number = (int)strtol(longIntData, NULL, 16);
                // printf("\n  (DEBUG (LONG INT CALCULATOR) Converted value (x c i s): '%x' '%c' '%i' '%s'", number, number, number, number);
                arrOfIntLen[arrOfIntLenIndex++] = number;
                // printf("\n  (DEBUG (LONG INT CALCULATOR) (arrOfIntLen): '%x' '%c' '%i' '%s'", arrOfIntLen[arrOfIntLenIndex-1], arrOfIntLen[arrOfIntLenIndex-1], arrOfIntLen[arrOfIntLenIndex-1], arrOfIntLen[arrOfIntLenIndex-1]);
            }
        }
    }
    printf("\n (DEBUG (LONG INT CALCULATOR) << longIntCalculator finished.");
}

void longLenCalculator(int lenToCalc)
{
    // NOTE: When the full packet is over 171 bytes (127 bytes of SNMP),
    // the packet is malformed because the size of the full SNMP packet
    // needs to be represented by two bytes (similar as when parsing a big OID).

    // Encoding a length greater than 127 (60,001): (Note: greater lengths than 65,535 are impractical and not used)
    // ---------------------------------------------
    // Octet 1  | Octet 2  | Octet 3
    // 10000010 | 11101010 | 01100001 (binary)
    // 82       | EA       | 61       (hexadecimal)

    // if length > 127 < 256
    // 		Octet 1 = 0x81
    // 		Octet 2 = length
    // if length >= 256
    // 		Octet 1 = 0x82
    // 		Octet 2 = length/128/2 = 60001/128/2 = 0xEA (the division of integers -not taking account reminder-)
    // 		Octet 3 = length-(length/128/2*128*2) = 0x61 (the multiplication and division of integers -not taking account reminder-)

    free(arrOfLen);
    calcLenLen = 0;

    // memset(arrOfLen, '\0', strlen(arrOfLen));
    unsigned int j = 0;
    if (lenToCalc <= 127)
    {
        // printf("\n >> longLenCalculator >> Processing length of %i", lenToCalc);
        arrOfLen[j++] = lenToCalc; // The value needs to be converted to int, so the hexa is OK
        calcLenLen++;
        // printf("\n >> longLenCalculator >> arrOfLen[%i]: %x", j-1, arrOfLen[j-1]);
    }
    else if (127 < lenToCalc) // Most significant bit must indicate short o long number coming
    {
        // printf("\n >> longLenCalculator >> Processing length of %i", lenToCalc);
        if (lenToCalc < 256) // Using 1 extra octet
        {
            arrOfLen[j++] = 0x81;      // 1000 0001 (1)[........]
            // printf("\n >> longLenCalculator >> arrOfLen[%i]: %x", j-1, arrOfLen[j-1]);
            arrOfLen[j++] = lenToCalc; // (1)[........]
            // printf("\n >> longLenCalculator >> arrOfLen[%i]: %x", j-1, arrOfLen[j-1]);
            calcLenLen += 2;
        }
        else if (256 <= lenToCalc && lenToCalc < 65536) // Using 2 extra octets
        {
            arrOfLen[j++] = 0x82;            // 1000 0010 (1)[........] (2)[........]
            // printf("\n >> longLenCalculator >> arrOfLen[%i]: %x", j-1, arrOfLen[j-1]);
            arrOfLen[j++] = lenToCalc / 256; // (1)[........]
            // printf("\n >> longLenCalculator >> arrOfLen[%i]: %x", j-1, arrOfLen[j-1]);
            arrOfLen[j++] = lenToCalc % 256; // (2)[........]
            // printf("\n >> longLenCalculator >> arrOfLen[%i]: %x", j-1, arrOfLen[j-1]);
            calcLenLen += 3;
        }
        else
        {
            printf("\n (!!) Error: the length of field (%i) is over 65,536 and it's not supported.", lenToCalc);
        }
    }
}

void bigOidConverter(unsigned int originalOid)
{
    oidConvertedLen = 0;

    // printf("\n (DEBUG) (BIG OID CONVERTER) +++ Converting OID - --------------------------");
    // printf("\n (DEBUG) (BIG OID CONVERTER) +++ Converting OID - originalOid: %i (i) %x (x)", originalOid, originalOid);
    if (originalOid <= 127)
    {
        oidConvertedVal[0] = originalOid;
        oidConvertedLen++;
        // printf("\n (DEBUG) (BIG OID CONVERTER) +++ Converting OID - oidConvertedVal[0]: %i (i) %x (x)", oidConvertedVal[0], oidConvertedVal[0]);
    }
    else if (127 < originalOid && originalOid <= 16384) // Using the 7 les significant bits of two bytes: 2^(7+7) = 16,384
    {
        oidConvertedVal[0] = (originalOid / 128) + 128; // Division of integers
        oidConvertedVal[1] = originalOid % 128;         // The same as (originalOid - (originalOid / 128 * 128)). Division and multiplication of integers.
        oidConvertedLen += 2;
        // printf("\n (DEBUG) (BIG OID CONVERTER) +++ Converting OID - oidConvertedVal[0]: %i (i) %x (x)", oidConvertedVal[0], oidConvertedVal[0]);
        // printf("\n (DEBUG) (BIG OID CONVERTER) +++ Converting OID - oidConvertedVal[1]: %i (i) %x (x)", oidConvertedVal[1], oidConvertedVal[1]);
    }
    else if (16384 < originalOid && originalOid < 2097152) // Using the 7 les significant bits of three bytes: 2^(7+7+7) = 2,097,152
    {
        oidConvertedVal[0] = (originalOid / (128 * 128)) + 128;         // Division and multiplication of integers
        oidConvertedVal[1] = ((originalOid % (128 * 128)) / 128) + 128; // Division and multiplication of integers
        oidConvertedVal[2] = originalOid % 128;                         // The same as (originalOid - (originalOid / 128 * 128)). Division and multiplication of integers.
        oidConvertedLen += 3;
        // printf("\n (DEBUG) (BIG OID CONVERTER) +++ Converting OID - oidConvertedVal[0]: %i (i) %x (x)", oidConvertedVal[0], oidConvertedVal[0]);
        // printf("\n (DEBUG) (BIG OID CONVERTER) +++ Converting OID - oidConvertedVal[1]: %i (i) %x (x)", oidConvertedVal[1], oidConvertedVal[1]);
        // printf("\n (DEBUG) (BIG OID CONVERTER) +++ Converting OID - oidConvertedVal[2]: %i (i) %x (x)", oidConvertedVal[2], oidConvertedVal[2]);
    }
    else // Greater 2,097,152
    {
        printf("\n (!!) Error: the value of %i is over 2,097,151 and it's not supported.", originalOid);
    }
}

void readConfigData()
{
    FILE *fp1;
    char *fp1Line = NULL;
    size_t fp1Len = 0;
    ssize_t fp1Size;

    fp1 = fopen("/usr/local/snmp-trap.cfg", "r");
    if (fp1 == NULL)
    {
        printf("\n (DEBUG) (READ CONFIG DATA) !!! ERROR !!! Error opening file '/usr/local/snmp-trap.cfg'");
        flagErrorReadingFile = 1;
    }
    else
    {
        int j = 0;
        while ((fp1Size = getline(&fp1Line, &fp1Len, fp1)) != -1)
        {
            // Parsing fp1Line
            // int init_size = strlen(fp1Line);
            char delim[] = "=";
            char *ptr = strtok(fp1Line, delim);
            if (j == 0)
            { // IP
                int k = 0;
                while (ptr != NULL)
                {
                    if (k == 1)
                    {
                        ptr[strlen(ptr) - 1] = '\0'; // Remove new line char
                        strcpy(configDataIp, ptr);
                    }
                    ptr = strtok(NULL, delim);
                    k++;
                }
            }
            else if (j == 1)
            { // Port number
                int k = 0;
                while (ptr != NULL)
                {
                    if (k == 1)
                    {
                        ptr[strlen(ptr) - 1] = '\0'; // Remove new line char
                        strcpy(configDataPort, ptr);
                    }
                    ptr = strtok(NULL, delim);
                    k++;
                }
            }
            else if (j == 2)
            { // Community
                int k = 0;
                while (ptr != NULL)
                {
                    if (k == 1)
                    {
                        ptr[strlen(ptr) - 1] = '\0'; // Remove new line char
                        strcpy(configDataCommunity, ptr);
                    }
                    ptr = strtok(NULL, delim);
                    k++;
                }
            }
            else if (j == 3)
            { // Source port
                int k = 0;
                while (ptr != NULL)
                {
                    if (k == 1)
                    {
                        strcpy(configDataSourcePort, ptr);
                    }
                    ptr = strtok(NULL, delim);
                    k++;
                }
            }
            else
            {
                // Do nothing
            }
            j++;
        }
        free(fp1Line);
        fclose(fp1);
    }
}

void fetchOidDatabaseValue()
{
    // Example of file content (OID~type~value):
    // -------------------------------------------
    // .1.3.6.1.2.1.33.1.2.1.0~02~2
    // .1.3.6.1.2.1.33.1.2.2.0~02~60
    // .1.3.6.1.2.1.33.1.2.3.0~02~120
    // .1.3.6.1.2.1.33.1.2.4.0~02~240
    // .1.3.6.1.2.1.33.1.2.5.0~02~480
    // .1.3.6.1.2.1.33.1.2.6.0~02~960
    // .1.3.6.1.2.1.33.1.2.7.0~02~90

    FILE *fp2;
    char *fp2Line = NULL;
    size_t fp2Len = 0;
    ssize_t fp2Size;

    fp2 = fopen("/usr/local/snmp-database.cfg", "r");
    if (fp2 == NULL)
    {
        printf("\n (DEBUG) (FETCH OID DATABASE VALUE) !!! ERROR !!! Error opening file '/usr/local/snmp-database.cfg'");
        flagErrorReadingDatabaseFile = 1;
    }
    else
    {
        int j = 0;
        while ((fp2Size = getline(&fp2Line, &fp2Len, fp2)) != -1)
        {
            // Parsing fp2Line
            // int init_size = strlen(fp2Line);
            char delim[] = "~";
            char *ptr = strtok(fp2Line, delim);
            unsigned int k = 0;
            while (ptr != NULL)
            {
                if (k == 0) // OID
                {
                    strcpy(snmpUpsDatabase_oid[j], ptr);
                }
                else if (k == 1) // Value
                {
                    ptr[strlen(ptr) - 1] = '\0'; // Remove new line char
                    strcpy(snmpUpsDatabase_value[j], ptr);
                }
                else
                {
                    // For future data
                }
                ptr = strtok(NULL, delim);
                k++;
            }
            j++;
        }
        snmpUpsDatabase_totalData = j;
        free(fp2Line);
        fclose(fp2);

        // DEBUG
        printf("\n (DEBUG) (FETCH OID DATABASE VALUE) ~~~~~~~~~~ snmpDatabase ~~~~~~~~~~");
        int i;
        for (i = 0; i < snmpUpsDatabase_totalData; i++)
        {
            printf("\n (DEBUG) (FETCH OID DATABASE VALUE) snmpUpsDatabase_oid[%i]=%s", i, snmpUpsDatabase_oid[i]);
        }
        for (i = 0; i < snmpUpsDatabase_totalData; i++)
        {
            printf("\n (DEBUG) (FETCH OID DATABASE VALUE) snmpUpsDatabase_value[%i]=%s", i, snmpUpsDatabase_value[i]);
        }
        printf("\n (DEBUG) (FETCH OID DATABASE VALUE) ~~~~~~~~~~ snmpDatabase ~~~~~~~~~~");
    }
}

void fetchOidListValue()
{
    // Example of file content (OID~type~value):
    // -------------------------------------------
    // .1.3.6.1.2.1.33.1.1.1.0;04;read-only
    // .1.3.6.1.2.1.33.1.1.2.0;04;read-only
    // .1.3.6.1.2.1.33.1.1.3.0;04;read-only
    // .1.3.6.1.2.1.33.1.1.4.0;04;read-only
    // .1.3.6.1.2.1.33.1.1.5.0;04;read-only
    // .1.3.6.1.2.1.33.1.1.6.0;04;read-only
    // .1.3.6.1.2.1.33.1.2.1.0;02;read-only
    // .1.3.6.1.2.1.33.1.2.2.0;02;read-only

    FILE *fp3;
    char *fp3Line = NULL;
    size_t fp3Len = 0;
    ssize_t fp3Size;

    fp3 = fopen("/usr/local/snmp-oid-list.cfg", "r");
    if (fp3 == NULL)
    {
        printf("\n (DEBUG) (FETCH OID LIST VALUE) !!! ERROR !!! Error opening file '/usr/local/snmp-oid-list.cfg'");
        flagErrorReadingOidListFile = 1;
    }
    else
    {
        int j = 0;
        while ((fp3Size = getline(&fp3Line, &fp3Len, fp3)) != -1)
        {
            // Parsing fp3Line
            // int init_size = strlen(fp3Line);
            char delim[] = ";";
            char *ptr = strtok(fp3Line, delim);
            unsigned int k = 0;
            while (ptr != NULL)
            {
                if (k == 0) // OID
                {
                    strcpy(snmpOidMibDatabase_oid[j], ptr);
                }
                else if (k == 1) // Type
                {
                    strcpy(snmpOidMibDatabase_type[j], ptr);
                }
                else if (k == 2) // Read Option
                {
                    ptr[strlen(ptr) - 1] = '\0'; // Remove new line char
                    strcpy(snmpOidMibDatabase_readOption[j], ptr);
                }
                else
                {
                    // For future data
                }
                ptr = strtok(NULL, delim);
                k++;
            }
            j++;
        }
        snmpOidMibDatabase_totalData = j;
        free(fp3Line);
        fclose(fp3);

        // DEBUG
        printf("\n (DEBUG) (FETCH OID LIST VALUE) ~~~~~~~~~~ snmpOidList ~~~~~~~~~~");
        int i;
        for (i = 0; i < snmpOidMibDatabase_totalData; i++)
        {
            printf("\n (DEBUG) (FETCH OID LIST VALUE) snmpOidMibDatabase_oid[%i]=%s", i, snmpOidMibDatabase_oid[i]);
        }
        for (i = 0; i < snmpOidMibDatabase_totalData; i++)
        {
            printf("\n (DEBUG) (FETCH OID LIST VALUE) snmpOidMibDatabase_type[%i]=%s", i, snmpOidMibDatabase_type[i]);
        }
        for (i = 0; i < snmpOidMibDatabase_totalData; i++)
        {
            printf("\n (DEBUG) (FETCH OID LIST VALUE) snmpOidMibDatabase_readOption[%i]=%s", i, snmpOidMibDatabase_readOption[i]);
        }
        printf("\n (DEBUG) (FETCH OID LIST VALUE) ~~~~~~~~~~ snmpOidList ~~~~~~~~~~");
    }
}

void oidParser(char oid[OID_LEN])
{
    char oidAux[OID_LEN];
    sprintf(oidAux, "%s", oid); // Make a copy of argument argvGlobal[3] so it is not overwriten

    // Reset values
    memset(oidVal, 0, strlen(oidVal));
    strcpy(oidVal, "");
    oidLen = 0;
    unsigned char *ptr = strtok(oid, "."); // 'strtok' modifies the original string 'oid'
    oidVal[0] = 0x2b;                      // '1.3' (fixed)
    oidLen = 1;                            // '1.3' (fixed)
    int j = 0;
    int over127 = 0;

    while (ptr != NULL)
    {
        // printf("\n Iteration (%i)", j);
        int ptr2int = atoi(ptr);
        // printf("\n >> oidParser >> ptr2int: %i %c %x", ptr2int);
        if (j > 1) // '1.3' (fixed)
        {
            if (ptr2int <= 127)
            {
                oidVal[j - 1 + over127] = ptr2int; // The value needs to be converted to int, so the hexa is OK
                // printf("\n >> oidParser >> oidVal[%i]: %x", j-1+over127, oidVal[j - 1 + over127]);
                oidLen++;
            }
            else if (127 < ptr2int && ptr2int <= 16384) // Using the 7 les significant bits of two bytes: 2^(7+7) = 16,384
            {
                oidVal[j - 1] = (ptr2int / 128) + 128; // Division of integers
                oidVal[j] = ptr2int % 128;             // The same as (ptr2int - (ptr2int / 128 * 128)). Division and multiplication of integers.
                // printf("\n >> oidParser >> oidVal[%i]: %x", j-1, oidVal[j - 1]);
                // printf("\n >> oidParser >> oidVal[%i]: %x", j, oidVal[j]);
                oidLen += 2;
                over127++;
            }
            else if (16384 < ptr2int && ptr2int < 2097152) // Using the 7 les significant bits of three bytes: 2^(7+7+7) = 2,097,152
            {
                oidVal[j - 1] = (ptr2int / (128 * 128)) + 128;     // Division and multiplication of integers
                oidVal[j] = ((ptr2int % (128 * 128)) / 128) + 128; // Division and multiplication of integers
                oidVal[j + 1] = ptr2int % 128;                     // The same as (ptr2int - (ptr2int / 128 * 128)). Division and multiplication of integers.
                // printf("\n >> oidParser >> oidVal[%i]: %x", j-1, oidVal[j - 1]);
                // printf("\n >> oidParser >> oidVal[%i]: %x", j, oidVal[j]);
                // printf("\n >> oidParser >> oidVal[%i]: %x", j+1, oidVal[j+1]);
                oidLen += 3;
                over127 += 2;
            }
            else // Greater 2,097,152
            {
                printf("\n (!!) Error: the value of %i is over 2,097,151 and it's not supported.", ptr2int);
            }
        }
        // printf("\n oidVal[%i]: %i %c %x", j, oidVal[j-1], oidVal[j-1], oidVal[j-1]);
        ptr = strtok(NULL, ".");
        j++;
    }
    // DEBUG:
    // j = 0;
    // while ( oidVal[j] != NULL ) {
    // 	printf("\n - function - oidVal[%i] = 0x%x", j, oidVal[j]);
    // 	j++;
    // }
    // for ( j = 0; j < strlen(oidVal)+3; j++)
    // {
    // 	printf("\n - function - oidVal[%i] = 0x%x", j, oidVal[j]);
    // }
    // printf("\n - - - - - - - - - - - - - - - - ");
}

void buildVarbind_Oid(unsigned char oidListInput[OID_LEN])
{
    unsigned int j = 0;
    unsigned int oidIndex = 0;

    unsigned char delim[2] = ".";
    unsigned char oidListAux[OID_LEN];
    unsigned char *token;

    varbindList.oid_value_hex_element[varbindList.index] = 0;
    varbindList.oid_len_hex_len[varbindList.index] = 0;
    varbindList.varbindCumulativeLen_hex_len[varbindList.index] = 0;
    varbindList.value_value_hex_len_hex_len[varbindList.index] = 0;
    flag_oidFoundUpsDatabase = 0;

    printf("\n (DEBUG) (BUILD VARBIND OID) OID iteration: (%i)", oidListIndex);

    // SECTION 1 ///// Get the OID   /////

    findMatchingOid(oidListInput);

    // SECTION 2 ///// Convert the value to correct type and get its hex representation /////

    // SECTION 2.1 ///// If no OID was found, then error /////

    if (flag_oidFoundUpsDatabase == 0)
    {
        // Use the original OID for the response
        strcpy(varbindList.oid_value[varbindList.index], oidListInput);
        // For GetNextRequest, if there is no next valid OID, respond with a 0x82 type of messaged attached to the original OID (of the GetNextRequest)
        if (snmpPduType == GET_NEXT_REQUEST || snmpPduType == GET_BULK_REQUEST)
        {
            // Setting the error response
            errorStatus = 0x00;
            errorIndex = 0x00;
            // Setting OID and value
            strcpy(varbindList.oid_value[varbindList.index], oidListInput);
            varbindList.value_type[varbindList.index] = ERROR_RESPONSE_END_OF_MIB_VIEW;
            // printf("\n (DEBUG) (BUILD VARBIND OID) (ERROR RESPONSE) errorStatus: %x - errorIndex: %x - varbindList.value_type[varbindList.index]: %x", errorStatus, errorIndex, varbindList.value_type[varbindList.index]);
        }
        else if (snmpPduType == GET_REQUEST)
        {
            errorStatus = 0x00;
            errorIndex = 0x00;
            // Setting OID and value
            strcpy(varbindList.oid_value[varbindList.index], oidListInput);
            varbindList.value_type[varbindList.index] = errorType;
        }
        else
        {
            // Setting the error response
            errorStatus = 0x02;
            errorIndex = oidListIndex + 1;
            // Setting value
            varbindList.value_type[varbindList.index] = 0x05; // Null
        }
        varbindList.value_value_hex_len[varbindList.index] = 0;
        varbindList.value_value[varbindList.index] = 0x00;
        varbindList.value_value_hex[varbindList.index][0] = 0x00;
    }

        // SECTION 2.2 ///// If OID found, convert value according to its type /////

    else
    {
        // Build the type and value to be sent in a response
        if (varbindList.value_type[varbindList.index] == 04 || varbindList.value_type[varbindList.index] == 23 || varbindList.value_type[varbindList.index] == 24 || varbindList.value_type[varbindList.index] == 27 || varbindList.value_type[varbindList.index] == 32 ||
            varbindList.value_type[varbindList.index] == 33 || varbindList.value_type[varbindList.index] == 37 || varbindList.value_type[varbindList.index] == 38 || varbindList.value_type[varbindList.index] == 40 || varbindList.value_type[varbindList.index] == 45) // All possible text-like values
        {
            varbindList.value_value_hex_len[varbindList.index] = strlen(varbindList.value_value_hex[varbindList.index]); // Len of value's array
            printf("\n (DEBUG) (BUILD VARBIND OID) (VALUE RESPONSE) varbindList.value_type[varbindList.index]: %x(x)", varbindList.value_type[varbindList.index]);
            printf("\n (DEBUG) (BUILD VARBIND OID) (VALUE RESPONSE) varbindList.value_value_hex[varbindList.index]: %s(s)", varbindList.value_value_hex[varbindList.index]);
            printf("\n (DEBUG) (BUILD VARBIND OID) (VALUE RESPONSE) varbindList.value_value_hex_len[varbindList.index]: %i", varbindList.value_value_hex_len[varbindList.index]);
        }
        else if (varbindList.value_type[varbindList.index] == 06 || varbindList.value_type[varbindList.index] == 26) // OBJECT IDENTIFIER
        {
            oidParser(varbindList.value_value_hex[varbindList.index]);
            strcpy(varbindList.value_value_hex[varbindList.index], "");
            unsigned int m;
            for (m = 0; m < oidLen; m++)
            {
                varbindList.value_value_hex[varbindList.index][m] = oidVal[m];
                printf("\n (DEBUG) (BUILD VARBIND OID) (VALUE RESPONSE) varbindList.value_value_hex[varbindList.index]: %x(x)", varbindList.value_value_hex[varbindList.index][m]);
            }
            varbindList.value_value_hex_len[varbindList.index] = oidLen;
            printf("\n (DEBUG) (BUILD VARBIND OID) (VALUE RESPONSE) varbindList.value_type[varbindList.index]: %x(x)", varbindList.value_type[varbindList.index]);
            printf("\n (DEBUG) (BUILD VARBIND OID) (VALUE RESPONSE) varbindList.value_value_hex_len[varbindList.index]: %i", varbindList.value_value_hex_len[varbindList.index]);
        }
        else
        {
            // Integer-like value
            varbindList.value_value[varbindList.index] = atoi(varbindList.value_value_hex[varbindList.index]); // Actual value
            // Considering big integers
            longIntCalculator(varbindList.value_value[varbindList.index]);
            for (j = 0; j < arrOfIntLenIndex; j++)
            {
                varbindList.value_value_hex[varbindList.index][j] = arrOfIntLen[j]; // Long int array
                printf("\n (DEBUG) (BUILD VARBIND OID) (VALUE RESPONSE) varbindList.value_value_hex[varbindList.index][%i]: %x(x)", j, varbindList.value_value_hex[varbindList.index][j]);
            }
            varbindList.value_value_hex_len[varbindList.index] = arrOfIntLenIndex; // Len of long int array
            // The len of this value should not be greater than 127
            printf("\n (DEBUG) (BUILD VARBIND OID) (VALUE RESPONSE) varbindList.value_type[varbindList.index]: %x(x)", varbindList.value_type[varbindList.index]);
            printf("\n (DEBUG) (BUILD VARBIND OID) (VALUE RESPONSE) varbindList.value_value[varbindList.index]: %i(i)", varbindList.value_value[varbindList.index]);
            printf("\n (DEBUG) (BUILD VARBIND OID) (VALUE RESPONSE) varbindList.value_value_hex_len[varbindList.index]: %i", varbindList.value_value_hex_len[varbindList.index]);
        }
    }

    // SECTION 3 ///// Convert the OID into hex /////

    // OID type will be constant
    varbindList.oid_type_hex[varbindList.index][0] = SNMP_BER_OBJECT_IDENTIFIER;

    // Parse the found OID to build it into the response message
    varbindList.oid_value_hex[varbindList.index][0] = 0x2b; // Corresponds to starting .1.3
    strcpy(oidListAux, varbindList.oid_value[varbindList.index]);
    token = strtok(oidListAux, delim); // This destroys the original stored value, so a copy is made
    while (token != NULL)
    {
        if (oidIndex > 1) // Not consider starting .1.3
        {
            // Considering the case in which the node value of the OID is big
            bigOidConverter(atoi(token));
            for (j = 0; j < oidConvertedLen; j++)
            {
                varbindList.oid_value_hex[varbindList.index][1 + varbindList.oid_value_hex_element[varbindList.index]++] = oidConvertedVal[j];
            }
        }
        token = strtok(NULL, delim);
        oidIndex++;
    }
    varbindList.oid_value_hex_elementLen[varbindList.index] = varbindList.oid_value_hex_element[varbindList.index] + 1;

    // === 1.3. Build TLV buffer for OIDs and values ===
    // === 1.3.1. Consider long lengths ===
    // Len of OID
    longLenCalculator(varbindList.oid_value_hex_elementLen[varbindList.index]);
    for (i = 0; i < calcLenLen; i++)
    {
        varbindList.oid_len_hex[varbindList.index][i] = arrOfLen[i];
        varbindList.oid_len_hex_len[varbindList.index]++;
    }

    longLenCalculator(varbindList.value_value_hex_len[varbindList.index]);
    for (i = 0; i < calcLenLen; i++)
    {
        varbindList.value_value_hex_len_hex[varbindList.index][i] = arrOfLen[i];
        varbindList.value_value_hex_len_hex_len[varbindList.index]++;
    }

    varbindList.varbindCumulativeLen[varbindList.index] = 1 + varbindList.oid_len_hex_len[varbindList.index] + varbindList.oid_value_hex_elementLen[varbindList.index] + 1 + varbindList.value_value_hex_len_hex_len[varbindList.index] + varbindList.value_value_hex_len[varbindList.index];

    // === 1.3.3. Varbind sequence ===
    // ... +-----+-----+-----+-------+-----+*****+*****+-----+-------+
    // ... | Seq | Len | OID | Value | ... | Seq | Len | OID | Value |
    // ... +-----+-----+-----+-------+-----+*****+*****+-----+-------+
    longLenCalculator(varbindList.varbindCumulativeLen[varbindList.index]);
    for (i = 0; i < calcLenLen; i++)
    {
        varbindList.varbindCumulativeLen_hex[varbindList.index][i] = arrOfLen[i];
        varbindList.varbindCumulativeLen_hex_len[varbindList.index]++;
    }
    arrayIndex = varbindList.varbindCumulativeLen[varbindList.index];
    varbindList.varbindVarbindLen[varbindList.index] = varbindList.varbindCumulativeLen[varbindList.index] + 1 + varbindList.varbindCumulativeLen_hex_len[varbindList.index];
}

void buildFinalBuffer() {

    unsigned int j, k;
    unsigned int indexStart;
    unsigned int snmpPduLenArrayLen = 0;
    unsigned int varbindTotalLenArrayLen = 0;
    unsigned int snmpCommLenArrayLen = 0;
    unsigned int snmpFullLenArrayLen = 0;
    unsigned int totalBulkOids = getBulk_NonRepeaters + getBulk_MaxRepetitions * (oidListTotal - getBulk_NonRepeaters);
    unsigned int repeatingOids = oidListTotal - getBulk_NonRepeaters;

    unsigned char varbindTotalLenArray[LEN_ARRAY];
    unsigned char snmpPduLenArray[LEN_ARRAY];
    unsigned char snmpCommLenArray[LEN_ARRAY];
    unsigned char snmpFullLenArray[LEN_ARRAY];

    // SECTION 1 ///// Build the final buffer /////

    // SECTION 1.1 ///// Assemble varbind list /////

    if (snmpPduType == GET_REQUEST || snmpPduType == GET_NEXT_REQUEST)
    {
        printf("\n (DEBUG) (BUILD FINAL BUFFER) ~~~~~~~~~~~~~~ Get or GetNext");
        for (i = 0; i < oidListTotal; i++)
        {
            printf("\n (DEBUG) (BUILD FINAL BUFFER) Non Repeaters");
            // Displace the array to make room for the new varbind at the beginning
            if (i > 0) // Considering the case there are no repeaters
            {
                displacement = varbindList.varbindCumulativeLen[i];
                bufferPreviousLen = bufferLen;
                while (bufferPreviousLen > 0)
                {
                    responseBuffer[bufferPreviousLen - 1 + displacement] = responseBuffer[bufferPreviousLen - 1];
                    bufferPreviousLen--;
                }
            }

            buildTLV(i);
        }
        printf("\n (DEBUG) (BUILD FINAL BUFFER) ~~~~~~~~~~~~~~ Get or GetNext");

        printf("\n (DEBUG) (BUILD FINAL BUFFER) Assembled buffer (bufferLen = %i):", bufferLen);

        for (i = 0; i < bufferLen; i++)
        {
            printf(" %x", responseBuffer[i]);
        }
    }
    else if (snmpPduType == GET_BULK_REQUEST)
    {
        printf("\n (DEBUG) (BUILD FINAL BUFFER) ~~~~~~~~~~~~~~ GetBulk");

        // Repeaters
        for (i = 0; i < getBulk_MaxRepetitions; i++)
        {
            // for (j = 0; j < getBulk_MaxRepetitions; j++)
            for (j = 0; j < repeatingOids; j++)
            {
                unsigned int oidBulkIndex = ((getBulk_MaxRepetitions - i) * repeatingOids) - (repeatingOids - j);

                // Displace the array to make room for the new varbind at the beginning
                if (j > 0 || i > 0)
                {
                    displacement = varbindList.varbindCumulativeLen[oidBulkIndex];
                    bufferPreviousLen = bufferLen;
                    while (bufferPreviousLen > 0)
                    {

                        responseBuffer[bufferPreviousLen - 1 + displacement] = responseBuffer[bufferPreviousLen - 1];
                        bufferPreviousLen--;
                    }
                }

                buildTLV(oidBulkIndex);
            }
        }

        // Non Repeaters
        indexStart = totalBulkOids - getBulk_NonRepeaters;
        for (i = indexStart; i < totalBulkOids; i++)
        {
            // Displace the array to make room for the new varbind at the beginning
            if (i > 0) // Considering the case there are no repeaters
            {
                displacement = varbindList.varbindCumulativeLen[i];
                bufferPreviousLen = bufferLen;

                while (bufferPreviousLen > 0)
                {
                    responseBuffer[bufferPreviousLen - 1 + displacement] = responseBuffer[bufferPreviousLen - 1];
                    bufferPreviousLen--;
                }
            }

            buildTLV(i);

        }
        printf("\n (DEBUG) (BUILD FINAL BUFFER) ~~~~~~~~~~~~~~ GetBulk");

        printf("\n (DEBUG) (BUILD FINAL BUFFER) Assembled buffer (bufferLen = %i):", bufferLen);

        for (i = 0; i < bufferLen; i++)
        {
            printf(" %x", responseBuffer[i]);
        }
    }
    else
    {
        // $$ FIXME $$ Error
    }

    // SECTION ///// Assemble rest of SNMP message /////

    // === 2. Varbind list sequence ===
    // ... +*****+*****+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    // ... | Seq | Len | Seq | Len | OID | Value | ... | Seq | Len | OID | Value |
    // ... +*****+*****+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    // Make room at the front of array
    longLenCalculator(bufferLen);
    for (i = 0; i < calcLenLen; i++)
    {
        varbindTotalLenArray[i] = arrOfLen[i];
        varbindTotalLenArrayLen++;
    }
    arrayIndex = bufferLen;
    while (arrayIndex >= 0)
    {
        responseBuffer[arrayIndex - 1 + varbindTotalLenArrayLen + 1] = responseBuffer[arrayIndex - 1];
        arrayIndex--;
    }
    // Write header into array
    responseBuffer[0] = SNMP_BER_SEQUENCE;
    for (i = 0; i < varbindTotalLenArrayLen; i++)
    {
        responseBuffer[i + 1] = varbindTotalLenArray[i];
    }
    bufferLen = bufferLen + 1 + varbindTotalLenArrayLen;

    // === DEBUG ===
    // printf("\n (DEBUG) (BUILD FINAL BUFFER) >>> responseBuffer = ");
    // for (i = 0; i < bufferLen; i++)
    // {
    //     printf("'%x' ", responseBuffer[i]);
    // }

    // === 3. Error check ===
    // ... +*****+*****+*******+*****+*****+*******+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    // ... | Err | Len | Value | Err | Len | Value | Seq | Len | Seq | Len | OID | Value | ... | Seq | Len | OID | Value |
    // ... +*****+*****+*******+*****+*****+*******+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    arrayIndex = bufferLen;
    // Make room at the front of array
    while (arrayIndex >= 0)
    {
        responseBuffer[arrayIndex - 1 + 6] = responseBuffer[arrayIndex - 1];
        arrayIndex--;
    }

    // Error status
    responseBuffer[0] = SNMP_BER_INTEGER;
    responseBuffer[1] = 0x01; // fixed
    // responseBuffer[2] = 0x00;
    responseBuffer[2] = errorStatus;

    // Error index
    responseBuffer[3] = SNMP_BER_INTEGER;
    responseBuffer[4] = 0x01; // fixed
    // responseBuffer[5] = 0x00;
    responseBuffer[5] = errorIndex;

    bufferLen = bufferLen + 6; // Error values should not go over 127

    // === 4. Request ID ===
    // ... +********+*****+*******+-----+-----+-------+-----+-----+-------+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    // ... | Req ID | Len | Value | Err | Len | Value | Err | Len | Value | Seq | Len | Seq | Len | OID | Value | ... | Seq | Len | OID | Value |
    // ... +********+*****+*******+-----+-----+-------+-----+-----+-------+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    arrayIndex = bufferLen;
    // Make room at the front of array
    while (arrayIndex >= 0)
    {
        responseBuffer[arrayIndex - 1 + reqIdLen + 2] = responseBuffer[arrayIndex - 1];
        arrayIndex--;
    }
    responseBuffer[0] = SNMP_BER_INTEGER;
    responseBuffer[1] = reqIdLen; // Asuming length will not be over 127
    for (i = 0; i < reqIdLen; i++)
    {
        responseBuffer[i + 2] = reqIdVal[i]; // Value
    }
    bufferLen = bufferLen + 2 + reqIdLen;

    // === 5. SNMP GetResponse PDU ===
    // ... +*****+*****+--------+-----+-------+-----+-----+-------+-----+-----+-------+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    // ... | PDU | Len | Req ID | Len | Value | Err | Len | Value | Err | Len | Value | Seq | Len | Seq | Len | OID | Value | ... | Seq | Len | OID | Value |
    // ... +*****+*****+--------+-----+-------+-----+-----+-------+-----+-----+-------+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    longLenCalculator(bufferLen);
    for (i = 0; i < calcLenLen; i++)
    {
        snmpPduLenArray[i] = arrOfLen[i];
        snmpPduLenArrayLen++;
    }
    arrayIndex = bufferLen;
    while (arrayIndex >= 0)
    {
        responseBuffer[arrayIndex - 1 + snmpPduLenArrayLen + 1] = responseBuffer[arrayIndex - 1];
        arrayIndex--;
    }
    // Write header into array
    responseBuffer[0] = GET_RESPONSE;
    for (i = 0; i < snmpPduLenArrayLen; i++)
    {
        responseBuffer[i + 1] = snmpPduLenArray[i];
    }
    bufferLen = bufferLen + 1 + snmpPduLenArrayLen;

    // === 6. SNMP Community ===
    // ... +******+*****+*******+-----+-----+--------+-----+-------+-----+-----+-------+-----+-----+-------+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    // ... | Comm | Len | Value | PDU | Len | Req ID | Len | Value | Err | Len | Value | Err | Len | Value | Seq | Len | Seq | Len | OID | Value | ... | Seq | Len | OID | Value |
    // ... +******+*****+*******+-----+-----+--------+-----+-------+-----+-----+-------+-----+-----+-------+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+

    longLenCalculator(strlen(configDataCommunity));
    for (i = 0; i < calcLenLen; i++)
    {
        snmpCommLenArray[i] = arrOfLen[i];
        snmpCommLenArrayLen++;
    }
    arrayIndex = bufferLen;
    while (arrayIndex >= 0)
    {
        responseBuffer[arrayIndex - 1 + snmpCommLenArrayLen + strlen(configDataCommunity) + 1] = responseBuffer[arrayIndex - 1];
        arrayIndex--;
    }
    // Write header into array
    responseBuffer[0] = SNMP_BER_OCTET_STRING;
    for (i = 0; i < snmpCommLenArrayLen; i++)
    {
        responseBuffer[i + 1] = snmpCommLenArray[i];
    }
    for (i = 0; i < strlen(configDataCommunity); i++)
    {
        responseBuffer[snmpCommLenArrayLen + i + 1] = configDataCommunity[i];
    }
    bufferLen = bufferLen + 1 + snmpCommLenArrayLen + strlen(configDataCommunity);

    // === 7. SNMP version ===
    // ... +**********+*****+*******+------+-----+-------+-----+-----+--------+-----+-------+-----+-----+-------+-----+-----+-------+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    // ... | SNMP Ver | Len | Value | Comm | Len | Value | PDU | Len | Req ID | Len | Value | Err | Len | Value | Err | Len | Value | Seq | Len | Seq | Len | OID | Value | ... | Seq | Len | OID | Value |
    // ... +**********+*****+*******+------+-----+-------+-----+-----+--------+-----+-------+-----+-----+-------+-----+-----+-------+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    arrayIndex = bufferLen;
    while (arrayIndex >= 0)
    {
        responseBuffer[arrayIndex - 1 + 3] = responseBuffer[arrayIndex - 1];
        arrayIndex--;
    }
    // Write header into array
    responseBuffer[0] = SNMP_BER_INTEGER; // Type (fixed)
    responseBuffer[1] = 0x01;             // Length (fixed)
    responseBuffer[2] = 0x01;             // Value (fixed)
    bufferLen = bufferLen + 3;

    // === 8. SNMP message header ===
    // +*****+*****+----------+-----+-------+------+-----+-------+-----+-----+--------+-----+-------+-----+-----+-------+-----+-----+-------+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    // | Seq | Len | SNMP Ver | Len | Value | Comm | Len | Value | PDU | Len | Req ID | Len | Value | Err | Len | Value | Err | Len | Value | Seq | Len | Seq | Len | OID | Value | ... | Seq | Len | OID | Value |
    // +*****+*****+----------+-----+-------+------+-----+-------+-----+-----+--------+-----+-------+-----+-----+-------+-----+-----+-------+-----+-----+-----+-----+-----+-------+-----+-----+-----+-----+-------+
    longLenCalculator(bufferLen);
    for (i = 0; i < calcLenLen; i++)
    {
        snmpFullLenArray[i] = arrOfLen[i];
        snmpFullLenArrayLen++;
    }
    arrayIndex = bufferLen;
    while (arrayIndex >= 0)
    {
        responseBuffer[arrayIndex - 1 + snmpFullLenArrayLen + 1] = responseBuffer[arrayIndex - 1];
        arrayIndex--;
    }
    //     Write header into array
    responseBuffer[0] = SNMP_BER_SEQUENCE; // Type (fixed)
    for (i = 0; i < snmpFullLenArrayLen; i++)
    {
        responseBuffer[i + 1] = snmpFullLenArray[i];
    }
    bufferLen = bufferLen + 1 + snmpFullLenArrayLen;

    // === DEBUG ===
    printf("\n (DEBUG) (BUILD FINAL BUFFER) >>> Total length of message: %i", bufferLen);
    printf("\n (DEBUG) (BUILD FINAL BUFFER) >>> BUFFER >>> ");
    for (i = 0; i < bufferLen; i++)
    {
        printf("%x ", responseBuffer[i]);
    }

}

void buildTLV(unsigned int indexTLV)
{
    unsigned int z;
    bufferLenAux = 0;

    // Assemble varbind
    // printf("\n (DEBUG) (BUILD TLV) varbindList.oid_type_hex[%i]: %x", i, varbindList.oid_type_hex[indexTLV][0]);
    responseBuffer[bufferLenAux++] = varbindList.oid_type_hex[indexTLV][0];
    bufferLen++;
    printf("\n (DEBUG) (BUILD TLV) varbindList.oid_len_hex[%i]:", i);
    for (z = 0; z < varbindList.oid_len_hex_len[indexTLV]; z++)
    {
        printf(" %x", varbindList.oid_len_hex[indexTLV][z]);
        responseBuffer[bufferLenAux++] = varbindList.oid_len_hex[indexTLV][z];
        bufferLen++;
    }
    printf("\n (DEBUG) (BUILD TLV) varbindList.oid_value_hex[%i]:", i);
    for (z = 0; z < varbindList.oid_value_hex_elementLen[indexTLV]; z++)
    {
        printf(" %x", varbindList.oid_value_hex[indexTLV][z]);
        responseBuffer[bufferLenAux++] = varbindList.oid_value_hex[indexTLV][z];
        bufferLen++;
    }
    printf("\n (DEBUG) (BUILD TLV) varbindList.value_type[%i]: %x", i, varbindList.value_type[indexTLV]);
    responseBuffer[bufferLenAux++] = varbindList.value_type[indexTLV];
    bufferLen++;
    printf("\n (DEBUG) (BUILD TLV) varbindList.value_value_hex_len_hex[%i]:", i);
    for (z = 0; z < varbindList.value_value_hex_len_hex_len[indexTLV]; z++)
    {
        printf(" %x", varbindList.value_value_hex_len_hex[indexTLV][z]);
        responseBuffer[bufferLenAux++] = varbindList.value_value_hex_len_hex[indexTLV][z];
        bufferLen++;
    }
    printf("\n (DEBUG) (BUILD TLV) varbindList.value_value_hex[%i]:", i);
    for (z = 0; z < varbindList.value_value_hex_len[indexTLV]; z++)
    {
        printf(" %x", varbindList.value_value_hex[indexTLV][z]);
        responseBuffer[bufferLenAux++] = varbindList.value_value_hex[indexTLV][z];
        bufferLen++;
    }

    // Make room for the Sequence and Length
    displacement = 2;
    bufferPreviousLen = bufferLen;
    while (bufferPreviousLen > 0)
    {

        responseBuffer[bufferPreviousLen - 1 + displacement] = responseBuffer[bufferPreviousLen - 1];
        bufferPreviousLen--;
    }
    bufferLen+=2; // $$ FIXME $$ Does this contemplate the fact that can be over 128?

    // Sequence and Length
    responseBuffer[0] = SNMP_BER_SEQUENCE;
    for (z = 0; z < varbindList.varbindCumulativeLen_hex_len[indexTLV]; z++) // $$ FIXME $$ Does this contemplate the fact that can be over 128?
    {
        responseBuffer[z + 1] = varbindList.varbindCumulativeLen_hex[indexTLV][z];
    }

    ////////////
    printf("\n (DEBUG) (BUILD TLV) responseBuffer (bufferLen = %i):", bufferLen);
    // bufferLen = bufferLen + varbindList.varbindCumulativeLen[indexTLV];
    unsigned int k;
    for (k = 0; k < bufferLen; k++)
    {
        printf(" %x", responseBuffer[k]);
    }
}

void findMatchingOid(unsigned char oidListInput[OID_LEN])
{
    unsigned int m;
    unsigned int n;
    unsigned int o;
    unsigned int p;

    // If GET_REQUEST
    //     Check if input OID matches with any UPS OID database entry
    //     DONE
    // If GET_NEXT_REQUEST or GET_BULK_REQUEST
    //     Test node by node: take a node from input and check with one node level in MIB OID database
    //     If found
    //         Check if MIB OID database entry is contained into UPS OID database entry
    //         DONE

    // SECTION 1 ///// If GetRequest /////

    if (snmpPduType == GET_REQUEST)
    {
        printf("\n (DEBUG) (FIND MATCHING OID) (GET_REQUEST) Getting OID list from MIBs...");
        for (n = 0; n < snmpOidMibDatabase_totalData; n++)
        {
            // If the received OID (or its prefix) matches with a stored OID
            if (strstr(oidListInput, snmpOidMibDatabase_oid[n]) != NULL)
            {
                printf("\n (DEBUG) (FIND MATCHING OID) (GET_REQUEST) OID (1) (%s) found on the MIB database (%s)", oidListInput, snmpOidMibDatabase_oid[n]);
                // Get the OID on the UPS database
                for (m = 0; m < snmpUpsDatabase_totalData; m++)
                {
                    if (strcmp(oidListInput, snmpUpsDatabase_oid[m]) == 0)
                    {
                        printf("\n (DEBUG) (FIND MATCHING OID) (GET_REQUEST) OID (2) (%s) found on the UPS database (%s)", oidListInput, snmpUpsDatabase_oid[m]);

                        // If matches, get the OID, type and value
                        strcpy(varbindList.oid_value[varbindList.index], oidListInput); // Get the actual next OID
                        varbindList.value_type[varbindList.index] = (int)strtol(snmpOidMibDatabase_type[n], NULL, 16); // Get the type of next OID
                        strcpy(varbindList.value_value_hex[varbindList.index], snmpUpsDatabase_value[m]); // Actual value
                        flag_oidFoundUpsDatabase = 1;
                        break;
                    }
                }
                if (flag_oidFoundUpsDatabase == 0)
                {
                    printf("\n (DEBUG) (FIND MATCHING OID) (GET_REQUEST) ERROR in getting OID (%s) - NoSuchInstance (0x81)", oidListInput);
                    errorType = ERROR_NO_SUCH_INSTANCE;
                }

                break;
            }
            else
            {
                printf("\n (DEBUG) (FIND MATCHING OID) (GET_REQUEST) ERROR in getting OID (%s) - NoSuchObject (0x80)", oidListInput);
                errorType = ERROR_NO_SUCH_OBJECT;
            }
            if (flag_oidFoundUpsDatabase == 1)
            {
                break;
            }
        }
    }

        // SECTION 2 ///// If GetNextRequest or GetBulkRequest /////

    else if (snmpPduType == GET_NEXT_REQUEST || snmpPduType == GET_BULK_REQUEST)
    {
        int matchIndex = -1;
        unsigned int oidToTrimLen = 0;
        unsigned int snmpOidMibDatabase_matchingPool = 0;
        unsigned int matchingOids = 0;

        unsigned char delimOid[2] = ".";
        unsigned char oidListInput_trimmed[OID_LEN];
        unsigned char matchingMibOid_trimmed[OID_LEN];
        unsigned char oidListInput_new[OID_LEN];
        unsigned char oidListInput_new_lastNode[MEDIUM_LEN];
        unsigned char snmpOidMibDatabase_oid_lastNode[MEDIUM_LEN];
        // unsigned char oidListInput_new2[OID_LEN];
        unsigned char *tokenOid;

        strcpy(oidListInput_new, oidListInput);

        // The objective is to find the index on the MIB database in which the comparison
        // between the MIB database and the UPS database should start.
        do
        {
            for (n = 0; n < snmpOidMibDatabase_totalData; n++)
            {
                printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) Main iteration: %i",n);
                if (snmpOidMibDatabase_matchingPool == 0)
                {
                    for (o = 0; o < snmpOidMibDatabase_totalData; o++)
                    {
                        if (strstr(snmpOidMibDatabase_oid[o], oidListInput_new))
                        {
                            snmpOidMibDatabase_matchingPool++;
                        }
                    }
                    printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) snmpOidMibDatabase_matchingPool: %i",snmpOidMibDatabase_matchingPool);
                }

                // Compare the number of nodes
                unsigned int oidListInput_new_nodeCount_int = 0;
                for(p = 0; oidListInput_new[p] != '\0'; p++)
                {
                    if(oidListInput_new[p] == '.')
                        ++oidListInput_new_nodeCount_int;
                }
                unsigned int snmpOidMibDatabase_oid_nodeCount_int = 0;
                for(p = 0; snmpOidMibDatabase_oid[n][p] != '\0'; p++)
                {
                    if(snmpOidMibDatabase_oid[n][p] == '.')
                        ++snmpOidMibDatabase_oid_nodeCount_int;
                }
                // printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) oidListInput_new_nodeCount_int:       %i",oidListInput_new_nodeCount_int);
                // printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) snmpOidMibDatabase_oid_nodeCount_int: %i",snmpOidMibDatabase_oid_nodeCount_int);

                if (strstr(snmpOidMibDatabase_oid[n], oidListInput_new))
                {
                    printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) Match of OID candidates: snmpOidMibDatabase_oid[%i]='%s' - oidListInput_new='%s'",n,snmpOidMibDatabase_oid[n],oidListInput_new);
                    if (oidToTrimLen > 0)
                    {
                        printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) There was a trimming...");
                        for (o = n; o < n + snmpOidMibDatabase_matchingPool; o++)
                        {
                            // if ()
                            // {
                                
                            // }
                            
                            // {
                            //     // If the input has more nodes than the MIB OIDs, compare them directly with no further trimming on the MIB OID.
                            //     if (strcmp(snmpOidMibDatabase_oid[n], oidListInput_new) == 0)
                            //     {
                            //         matchIndex = o;
                            //         break;
                            //     }
                            // }

                            if (oidListInput_new_nodeCount_int < snmpOidMibDatabase_oid_nodeCount_int)
                            {
                                // Get last node from the matching OID in the MIB database
                                strcpy(matchingMibOid_trimmed, snmpOidMibDatabase_oid[o]);
                                tokenOid = strtok(matchingMibOid_trimmed, delimOid);
                                for (p = 0; p < oidToTrimLen - 1; p++)
                                {
                                    strcat(matchingMibOid_trimmed, ".");
                                    unsigned char tokenChar[MEDIUM_LEN];
                                    strcpy(tokenChar, tokenOid);
                                    // strcat(matchingMibOid_trimmed, tokenOid); // <- This does not works. Maybe: https://stackoverflow.com/questions/38033041/printf-crashes-after-successful-printing
                                    strcat(matchingMibOid_trimmed, tokenChar);

                                    tokenOid = strtok(NULL, delimOid);

                                    strcpy(snmpOidMibDatabase_oid_lastNode, tokenOid);
                                }
                                // printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) snmpOidMibDatabase_oid_lastNode: %s", snmpOidMibDatabase_oid_lastNode);
                                // printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) oidListInput_new_lastNode:       %s", oidListInput_new_lastNode);
                                // If the MIB database OID last node is greater than the input OID last node, 
                                // that is the one to use (because the MIB OID is sorted)
                                if (atoi(snmpOidMibDatabase_oid_lastNode) > atoi(oidListInput_new_lastNode))
                                {
                                    matchIndex = o;
                                    printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) :::::::::::::: 0");
                                    break;
                                }
                            }
                            else
                            {
                                if (strcmp(snmpOidMibDatabase_oid[n], oidListInput_new) == 0)
                                {
                                    matchIndex = o;
                                    matchingOids = 1;
                                    printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) :::::::::::::: 1");
                                    break;
                                }
                                
                            }
                        }
                        if (matchIndex == -1)
                        {
                            printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) There was no trim...");
                            // If no one is greater, take the next OID
                            matchIndex = n + snmpOidMibDatabase_matchingPool + 1;
                            printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) :::::::::::::: 2");

                            // printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) matchIndex: %i", matchIndex);
                        }
                    }
                    else
                    {
                        matchIndex = n;
                                    printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) :::::::::::::: 3");

                        break;
                    }
                }

                if (matchIndex > -1)
                {
                    break;
                }
            }

            if (matchIndex == -1)
            {
                printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) (MATCH) There was no match...");
                // Get the number of nodes in the matching OID of the MIB OID database
                
                // oidToTrimLen = 0;
                // unsigned char oidListInput_new2[OID_LEN];
                // strcpy(oidListInput_new2, oidListInput_new);
                // tokenOid = strtok(oidListInput_new2, delimOid);
                // while( tokenOid != NULL ) {
                //     tokenOid = strtok(NULL, delimOid);
                //     oidToTrimLen++;
                // }
                // printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) oidToTrimLen: %i", oidToTrimLen);

                // unsigned char oidListInput_new2[OID_LEN];
                oidToTrimLen = 0;
                for(i = 0; oidListInput_new[i] != '\0'; i++)
                {
                    if(oidListInput_new[i] == '.')
                        ++oidToTrimLen;
                }
                // printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) oidToTrimLen: %i", oidToTrimLen);

                // Trim last node from the input OID
                // strcpy(oidListInput_trimmed, oidListInput_new);
                strcpy(oidListInput_trimmed, "");
                tokenOid = strtok(oidListInput_new, delimOid);
                for (o = 0; o < oidToTrimLen - 1; o++)
                {
                    strcat(oidListInput_trimmed, ".");
                    strcat(oidListInput_trimmed, tokenOid);

                    tokenOid = strtok(NULL, delimOid);

                    strcpy(oidListInput_new_lastNode, tokenOid);
                }
                strcpy(oidListInput_new,"");
                strcpy(oidListInput_new, oidListInput_trimmed);
                // printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) After trimming: oidListInput_trimmed: %s", oidListInput_trimmed);
                // printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) After trimming: oidListInput_new:     %s", oidListInput_new);

            }
            else
            {
                printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) (MATCH) There was a match!!!");
                
                // $$ FIXME $$ Working on this $$$$$$$$$$$$$$$$$$$$$$$$$$
                if (matchIndex == 0) // The input OID is smaller (or equal) than the first OID in the MIB and UPS databases
                {
                    for (o = 0; o < snmpUpsDatabase_totalData; o++)
                    {
                        if (matchingOids == 1)
                        {
                            for (n = matchIndex + 1; n < snmpOidMibDatabase_totalData - 1; n++)
                            {
                                if (strstr(snmpUpsDatabase_oid[o],snmpOidMibDatabase_oid[n]))
                                {
                                    printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) FINAL MATCH! (0) snmpUpsDatabase_oid[%i]=%s", o, snmpUpsDatabase_oid[o]);

                                    // If matches, get the OID, type and value
                                    strcpy(varbindList.oid_value[varbindList.index], snmpUpsDatabase_oid[o]); // Get the actual next OID
                                    varbindList.value_type[varbindList.index] = (int)strtol(snmpOidMibDatabase_type[n], NULL, 16); // Get the type of next OID
                                    strcpy(varbindList.value_value_hex[varbindList.index], snmpUpsDatabase_value[o]); // Actual value
                                    flag_oidFoundUpsDatabase = 1;
                                    break;
                                }
                            }
                            if (flag_oidFoundUpsDatabase == 1)
                            {
                                break;
                            }
                        }
                        else
                        {
                            if (strstr(snmpUpsDatabase_oid[o],snmpOidMibDatabase_oid[matchIndex]))
                            {
                                printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) FINAL MATCH! (1) snmpUpsDatabase_oid[%i]=%s", o, snmpUpsDatabase_oid[o]);

                                // If matches, get the OID, type and value
                                strcpy(varbindList.oid_value[varbindList.index], snmpUpsDatabase_oid[o]); // Get the actual next OID
                                varbindList.value_type[varbindList.index] = (int)strtol(snmpOidMibDatabase_type[matchIndex], NULL, 16); // Get the type of next OID
                                strcpy(varbindList.value_value_hex[varbindList.index], snmpUpsDatabase_value[o]); // Actual value
                                flag_oidFoundUpsDatabase = 1;
                                break;
                            }
                            if (flag_oidFoundUpsDatabase == 1)
                            {
                                break;
                            }
                        }
                    }
                }
                // $$ FIXME $$ Working on this $$$$$$$$$$$$$$$$$$$$$$$$$$
                else // The input OID is somewhere else in the MIB and UPS databases
                {
                    for (n = matchIndex; n < snmpOidMibDatabase_totalData - 1; n++)
                    {
                        // printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) Analysing index (%i) on the MIB OID database: %s", n, snmpOidMibDatabase_oid[n]);

                        for (o = 0; o < snmpUpsDatabase_totalData; o++)
                        {
                            printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) Analysing index (%i) on the MIB OID database - Checking against UPS OID database (%i) %s", n, o, snmpUpsDatabase_oid[o]);

                            // If the next OID matches
                            if (strstr(snmpUpsDatabase_oid[o],snmpOidMibDatabase_oid[n+1]))
                            {
                                // printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) Analysing index (%i) on the MIB OID database - (COMPARING) snmpUpsDatabase_oid[%i]=%s with snmpOidMibDatabase_oid[%i+1]=%s", n, o, snmpUpsDatabase_oid[o], n, snmpOidMibDatabase_oid[n+1]);
                                printf("\n (DEBUG) (FIND MATCHING OID) (GET_NEXT_REQUEST | GET_BULK_REQUEST) FINAL MATCH! (2) snmpUpsDatabase_oid[%i]=%s", o, snmpUpsDatabase_oid[o]);

                                // If matches, get the OID, type and value
                                strcpy(varbindList.oid_value[varbindList.index], snmpUpsDatabase_oid[o]); // Get the actual next OID
                                varbindList.value_type[varbindList.index] = (int)strtol(snmpOidMibDatabase_type[n+1], NULL, 16); // Get the type of next OID
                                strcpy(varbindList.value_value_hex[varbindList.index], snmpUpsDatabase_value[o]); // Actual value
                                flag_oidFoundUpsDatabase = 1;
                                break;
                            }
                        }

                        if (flag_oidFoundUpsDatabase == 1)
                        {
                            break;
                        }
                    }
                }
            }
        } while (matchIndex == -1);
    }
}