/* Test application which should print something when the remote
 * connects and sends data.
 */

#include <globaldefs.h>
#include <captureif.h>
#include <stdio.h>
#include <assert.h>

void RemoteConnected(void* _p)
{
   assert(_p);
   printf("Remote connected.\n");
}

void DataInd(void* _p, const char* _data, const int _size)
{
   assert(_p);
   assert(_data);
   printf("Remote data indication, %d bytes.\n", _size);
}

void RemoteDisconnected(void* _p)
{
   assert(_p);
   printf("Remote disconnected.\n");
}

int main(int argc, char *argv[])
{
   /* Address of the remote. */
   const char* destinationAddress = "00:19:C1:58:C3:B7";
   configuration config;
   captureData cd;
   void* p = (void*)0x1; /* Unused here. */
   int res = BDREMOTE_FAIL;
   InitCaptureData(&cd, 
                   &config, 
                   p, 
                   destinationAddress, 
                   30 /* timeout in secounds */);
   
   /* Run capture loop. */
   res = captureLoop(&cd);

  if (res == BDREMOTE_FAIL)
    {
      BDREMOTE_ERR("captureLoop failed.");
      return BDREMOTE_FAIL;
    }

  return BDREMOTE_OK;
}
