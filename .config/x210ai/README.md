# X210Ai power configuration

The user service selects `power-saver` on battery and `balanced` on AC through
`power-profiles-daemon`. It leaves ACPI wake sources and kernel power controls
untouched.

Bootstrap enables the system daemon and the user service. Check the service
with:

```sh
systemctl --user status x210ai-power.service
```
