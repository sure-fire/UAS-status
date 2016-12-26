Pebble.addEventListener('ready', function(e) {
  Pebble.sendAppMessage({'APP_READY': true});
});

Pebble.addEventListener('appmessage', function(dict) {
  queryState();
});

function queryState() {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    if(xhr.status == 200) {
      sendResultToPebble(xhr.response);
    }
  };
  xhr.open('GET', 'http://www.unallocatedspace.org/status', true);    // true = async mode
  xhr.send();
}

function sendResultToPebble(str) {
  if(str) {
    var state = str[0];
    var since = str.substr(str.indexOf('since ') + 6);

    var hr = since.substr(0,2);
    var min = since.substr(3,2);
    var sec = since.substr(6,2);
    var yr = since.substr(9,4);
    var mon = since.substr(14,2);
    var day = since.substr(17,2);
    
    console.log("HOUR: '" + hr + "'");
    console.log("MINUTE: " + min + "'");
    console.log("SECOND: '" +  sec + "'");
    console.log("YEAR: " + yr + "'");
    console.log("MONTH: '" + mon + "'");
    console.log("DAY: '" + day + "'");
    
    since = hr + ":" + min + " on " + mon + "/" + day
    
    /*
          [PHONE] pebble-app.js:?: UAS Status:134 ACQUIRED SINCE: 22:11:35 2016-12-21
          
          [PHONE] pebble-app.js:?: UAS Status:112 HOUR: '2'
          [PHONE] pebble-app.js:?: UAS Status:113 MINUTE: 11'
          [PHONE] pebble-app.js:?: UAS Status:114 SECOND: ':3'
          [PHONE] pebble-app.js:?: UAS Status:115 YEAR:  201'
          [PHONE] pebble-app.js:?: UAS Status:116 MONTH: '-1'
          [PHONE] pebble-app.js:?: UAS Status:117 DAY: '2-'
    */

    //console.log(yrs + "/" + mon + "/" + day + " " + hr + ":" + min + ":" + sec);
    
    console.log("ACQUIRED STATE: " + state);
    console.log("ACQUIRED SINCE: " + since);
    Pebble.sendAppMessage({
      'STATE': state,
      'SINCE': since
    });
  }
}