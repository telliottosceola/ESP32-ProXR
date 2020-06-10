#ifndef TARALIST_H
#define TARALIST_H

#include <NtpClientLib.h>

class Taralist{
public:
  void init();
  void loop();
private:
  void onNTPSynced(NTPSyncEvent_t ntpEvent);
}
#endif
