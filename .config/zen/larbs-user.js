// LARBS Zen performance profile.
user_pref("font.name.sans-serif.x-western", "Roboto");
user_pref("font.name.serif.x-western", "Roboto");
user_pref("font.name.monospace.x-western", "Roboto Mono");
user_pref("font.size.variable.x-western", 16);
user_pref("font.size.fixed.x-western", 14);
// Keep WebRender enabled for responsive browser performance.
user_pref("layers.acceleration.disabled", false);
user_pref("gfx.webrender.enabled", true);
user_pref("gfx.webrender.all", true);
user_pref("gfx.canvas.accelerated", true);
user_pref("media.hardware-video-decoding.enabled", true);
user_pref("media.hardware-video-decoding.force-enabled", true);
user_pref("widget.dmabuf.force-enabled", false);
// Use the XDG portal file picker, backed by lf in a floating terminal.
user_pref("widget.use-xdg-desktop-portal.file-picker", 1);
user_pref("dom.ipc.processCount", 4);
user_pref("browser.sessionhistory.max_total_viewers", 2);
user_pref("browser.sessionstore.interval", 60000);
user_pref("browser.tabs.unloadOnLowMemory", true);
user_pref("browser.cache.disk.enable", true);
user_pref("browser.cache.disk.capacity", 524288);
user_pref("browser.cache.memory.capacity", 65536);
user_pref("toolkit.telemetry.enabled", false);
user_pref("datareporting.healthreport.uploadEnabled", false);
user_pref("browser.ping-centre.telemetry", false);
user_pref("browser.newtabpage.activity-stream.feeds.telemetry", false);
user_pref("browser.newtabpage.activity-stream.telemetry", false);
