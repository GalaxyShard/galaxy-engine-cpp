// For WebGL
// https://stackoverflow.com/a/54627719
Module["preRun"].push(function() {
    FS.mkdir("/gamedata");
    FS.mount(IDBFS, {}, "/gamedata");
    FS.syncfs(true, function(err) {
        if (err) console.log("Error syncing file system: ", err);
    });
});