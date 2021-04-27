$1 -shared -o libhook.so hook.c -fPIC -ldl -D_GNU_SOURCE
$1 -o hook_ctl hook_ctl.c -L./ -lhook
