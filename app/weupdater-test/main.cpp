#include "weupdater.h"

//"v[0~999].[0~999].[0~999].[0~999]"
int main()
{
    WEUpdater weupdater;
    string productversion = "\"v0.9.0.0\"";
    if(weupdater.checkVersion("http://wifievent.io/version/netblock.txt", productversion))
    {
        cout << "something action to do in product(print notion that \'you have to update!\')\n";
    }
    else
    {
        cout << "there is no new version of product\n";
    }
    return 0;
}
