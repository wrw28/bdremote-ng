// Test application which should print something when the remote
// connects and sends data.

#include <globaldefs.h>
#include <captureif.h>
#include <stdio.h>

void RemoteConnected()
{
  printf("Remote connected.\n");
}

void DataInd(void* _p, const char* _data, const int _size)
{
  printf("Remote data indication, %d bytes.\n", _size);
}

void RemoteDisconnected()
{
  printf("Remote disconnected.\n");
}

int main(int argc, char *argv[])
{
  // Address of the remote.
  const char* destinationAddress = "00:19:C1:58:C3:B7";

  captureData cd;
  void* p = (void*)0x1; // Unused here.
  InitCaptureData(&cd, p, destinationAddress, 30 /* timeout in secounds */);

  // Run capture loop.
  int res = captureLoop(&cd);

  if (res == BDREMOTE_FAIL)
    {
      BDREMOTE_ERR("captureLoop failed.");
      return BDREMOTE_FAIL;
    }

  return BDREMOTE_OK;
}
