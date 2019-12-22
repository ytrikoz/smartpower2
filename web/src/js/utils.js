
function parseIP(ipaddress) { if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress)) { return ipaddress; } return ''; }

function convertLineBreaks(line) {
    if (line) {
      return line.replace(/(?:\r\n|\r|\n)/g, '<br>');
    }
    return '';
  }

function enableMainswitch(enabled = true) {
    if (enabled) {
      $('#mainswitch').removeClass('ui-state-disabled');
    } else {
      $('#mainswitch').addClass('ui-state-disabled');
    }
  }
  