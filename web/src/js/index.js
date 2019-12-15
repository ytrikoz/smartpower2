/* eslint-disable no-console */
/* eslint-disable max-len */
/* eslint-disable no-undef */

const GET_PAGE = 'p';
const SET_ONOFF = 'o';
const SET_MEASUREWATTHOUR = 'm';
const SET_POWER_MODE = 'a';
const SET_DEFAULT_VOLTAGE = 'v';
const SET_VOLTAGE = 'w';
const SET_NETWORK = 'n';
const FW_VERSION = 'f';
const TAG_PVI = 'd';
const SYS_INFO = 'S';
const NETWORK_INFO = 'N';
/*
const SET_SSID = 's';
const SET_IP_ADDR = 'i';
const SET_PASSWD = 'x';
*/
const PAGE_HOME = 1;
const PAGE_OPTIONS = 2;
const PAGE_INFO = 3;

let ws;

function parseIP(ipaddress) { if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress)) { return ipaddress; } return ''; }

function onVoltChange(item) {
  $('#volt_ss').sevenSeg({ digits: 5, decimalPlaces: 3, value: item.last });
}

function onAmpChange(item) {
  $('#amp_ss').sevenSeg({ digits: 5, decimalPlaces: 3, value: item.last });
}

function onWattChange(item) {
  $('#watt_ss').sevenSeg({ digits: 5, decimalPlaces: 3, value: item.last });
}

function onWatthChange(item) {
  if (item.last < 10) {
    $('#watth_ss').sevenSeg({ digits: 5, decimalPlaces: 3 });
  } else if (item.last < 100) {
    $('#watth_ss').sevenSeg({ digits: 5, decimalPlaces: 2 });
  } else if (item.last < 1000) {
    $('#watth_ss').sevenSeg({ digits: 5, decimalPlaces: 1 });
  } else {
    $('#watth_ss').sevenSeg({ digits: 5, decimalPlaces: 0 });
  }
  $('#watth_ss').sevenSeg({ value: item.last });
}

voltMeasurement = new Measurement('Volt', onVoltChange);
currentMeasurement = new Measurement('Amps', onAmpChange);
powerMeasurement = new Measurement('Watt', onWattChange);
watthMeasurement = new Measurement('Watth', onWatthChange);

// eslint-disable-next-line no-template-curly-in-string
const template = {
  '<>': 'li',
  html() {
    let key;
    for (key in this) if (Object.prototype.hasOwnProperty.call(this, key)) break;
    return `${key}<span>${this[key]}</span>`;
  },
};

function showOnOff(value) {
  $('#chk_onoff').val(value ? 'on' : 'off').flipswitch().flipswitch('refresh');
  if (value) {
    $('ui-icon-power').addClass('state-on');
  } else {
    $('ui-icon-power').addClass('state-off');
  }
}

function enableOnOff(enabled = true) {
  if (enabled) {
    $('#onoff').removeClass('ui-state-disabled');
  } else {
    $('#onoff').addClass('ui-state-disabled');
  }
}

function setHold(value) {
  $('#chk_hold').val(value ? 'on' : 'off').flipswitch().flipswitch('refresh');

  enableOnOff(!value);
}

function enableDashboard(enabled = true) {
  const $containers = $('#livedata .ss_cont');
  if (enabled) {
    $containers.removeClass('ui-state-disabled');
  } else {
    $containers.addClass('ui-state-disabled');
    $containers.sevenSeg().sevenSeg({ digits: 5, decimalPlaces: 3, value: null });
  }
}

function showPowerMode(value) {
  const $obj = $('input:radio[name="power_mode"]');
  $obj.prop('checked', false).checkboxradio().checkboxradio('refresh');
  $obj.filter(`[value="${value}"]`).prop('checked', true).checkboxradio('refresh');
}

function showOutputVoltage(value) {
  $('#slider_voltage').val(value).slider().slider('refresh');
}

function showEnableLog(value) {
  $('#chk_enable_log').val(value ? 'on' : 'off').flipswitch().flipswitch('refresh');
}

function getConnectionStatus() {
  try {
    return ws.readyState === WebSocket.OPEN;
  } catch (e) {
    return false;
  }
}

function store(param, value) {
  localStorage.setItem(param, value);
}

// eslint-disable-next-line no-unused-vars
function convertLineBreaks(line) {
  if (line) {
    return line.replace(/(?:\r\n|\r|\n)/g, '<br>');
  }
  return '';
}

function getPageIndex(title) {
  if (title === 'Home') {
    return PAGE_HOME;
  }
  if (title === 'Options') {
    return PAGE_OPTIONS;
  }
  if (title === 'Info') {
    return PAGE_INFO;
  }
  return -1;
}

function send(message) {
  console.log('>>>', message, getConnectionStatus());
  try {
    ws.send(message);
    return true;
  } catch (e) {
    return false;
  }
}

function showWiFi(value) {
  const $wifiMode = $('input:radio[name="wifi_mode"]');
  $wifiMode.prop('checked', false).checkboxradio().checkboxradio('refresh');
  $wifiMode.filter(`[value="${value}"]`).prop('checked', true).checkboxradio('refresh');
}

function showSSID(value) {
  $('#tf_ssid').val(value).textinput().textinput('refresh');
}

function showPassword(value) {
  $('#tf_passwd').val(value).textinput().textinput('refresh');
}

function showDHCP(value) {
  $('#chk_dhcp').val(value ? 'on' : 'off').flipswitch().flipswitch('refresh');
}

function showIPAddr(value) {
  $('#tf_ipaddr').val(value).textinput().textinput('refresh');
}

function showNetmask(value) {
  $('#tf_netmask').val(value).textinput().textinput('refresh');
}

function showGateway(value) {
  $('#tf_gateway').val(value).textinput().textinput('refresh');
}

function showDns(value) {
  $('#tf_dns').val(value).textinput().textinput('refresh');
}

function showVersion(value) {
  $('#version_info_cont').json2html(value, template, { replace: true });
}

function showNetworkInfo(value) {
  $('#network_info_cont').json2html(value, template, { replace: true });
}

function showSysInfo(value) {
  $('#system_info_cont').json2html(value, template, { replace: true });
}

function enableIpAddrFields(enabled = true) {
  if (enabled) {
    $('#ipaddr_cont').removeClass('ui-state-disabled');
  } else {
    $('#ipaddr_cont').addClass('ui-state-disabled');
  }
}

function enableUI(value = true) {
  if (value) {
    $('.ui-pagecontainer').removeClass('ui-disabled');
  } else {
    $('.ui-pagecontainer').addClass('ui-disabled');
  }
}

function sendOnOff(value) {
  // invert!
  send(`${SET_ONOFF}${value ? '0' : '1'}`);
}

function sendLogEnabled(value) {
  send(`${SET_MEASUREWATTHOUR}${value ? '1' : '0'}`);
}

function sendPowermode(value) {
  send(`${SET_POWER_MODE}${value}`);
}

function sendOutputVoltage(value, saveAsDefault = false) {
  send(`${SET_VOLTAGE}${value}`);
  if (saveAsDefault) {
    send(SET_DEFAULT_VOLTAGE);
  }
}

function sendNetwork(wifi, ssid, passwd, dhcp, ip, netmask, gateway, dns) {
  send(`${SET_NETWORK}${wifi},${ssid},${passwd},${dhcp ? '1' : '0'},${ip},${netmask},${gateway},${dns}`);
}

function updateUI(param, value) {
  switch (param) {
    case SET_ONOFF:
      showOnOff(!parseInt(value, 10));
      break;
    case SET_MEASUREWATTHOUR:
      showEnableLog(parseInt(value, 10));
      break;
    case SET_VOLTAGE:
      showOutputVoltage(parseFloat(value));
      break;
    case SET_POWER_MODE:
      showPowerMode(parseInt(value, 10));
      break;
    case SET_NETWORK: {
      const str = value.split(',');
      const wifi = parseInt(str[0], 10);
      const ssid = str[1];
      const passwd = str[2];
      const dhcp = parseInt(str[3], 2);
      const ip = parseIP(str[4]);
      const netmask = parseIP(str[5]);
      const gateway = parseIP(str[6]);
      const dns = parseIP(str[7]);
      showWiFi(wifi);
      showSSID(ssid);
      showPassword(passwd);
      showDHCP(dhcp);
      showIPAddr(ip);
      showNetmask(netmask);
      showGateway(gateway);
      showDns(dns);
      break;
    }
    case TAG_PVI: {
      const strArray = value.split(',');
      const volts = parseFloat(strArray[0]);
      const amps = parseFloat(strArray[1]);
      const watt = parseFloat(strArray[2]);
      const watth = parseFloat(strArray[3]);
      voltMeasurement.setValue(volts);
      currentMeasurement.setValue(amps);
      powerMeasurement.setValue(watt);
      watthMeasurement.setValue(watth);
      break;
    }
    case SYS_INFO: {
      showSysInfo(value);
      break;
    }
    case NETWORK_INFO: {
      showNetworkInfo(value);
      break;
    }
    case FW_VERSION:
      showVersion(value);
      break;
    default:
      console.log('unknown', param, value);
      break;
  }
}

function parseMessage(message) {
  console.log('<<<', message);

  const param = message.charAt(0);
  const value = message.substring(1);

  store(param, value);

  updateUI(param, value);
}

// eslint-disable-next-line no-unused-vars
function onload() {
  try {
    const scheme = window.location.protocol === 'https:' ? 'wss://' : 'ws://';
    const uri = `${scheme}${window.location.host}/ws`;
    console.log('connect', uri);
    ws = new WebSocket(uri);
    ws.onmessage = function onMessage(msg) {
      enableUI();
      parseMessage(msg.data);
    };
    ws.onopen = function onOpen() {
      const page = getPageIndex($('.ui-page-active').jqmData('title'));
      if (page > 0) {
        send(`${GET_PAGE}${[page]}`);
      }
    };
    ws.onclose = function onClose() {
      enableUI(false);
      setTimeout(onload, 1000);
    };
  } catch (e) {
    console.log(e);
  }
}

// eslint-disable-next-line no-unused-vars
function onunload() {
  localStorage.clear();
}

$(() => {
  $('[data-role="navbar"]').navbar();
  $('[data-role="toolbar"]').toolbar();
});

$(document).on('pagecontainerbeforeshow', (event, ui) => {
  const pageName = $(ui.toPage).jqmData('title');
  const page = getPageIndex(pageName);
  if (page > 0) {
    if (getConnectionStatus() && send(`${GET_PAGE}${[page]}`)) {
      $(ui.toPage).removeClass('ui-disabled');
    }
  }
});

$(document).on('pagecontainershow', (event, ui) => {
  const pageName = $(ui.toPage).jqmData('title');
  $('[data-role="navbar"] a.ui-button-active').removeClass('ui-button-active');
  $('#footer a').each((_index, element) => {
    if ($(element).text() === pageName) {
      $(element).addClass('ui-button-active');
    }
  });
});

$(document).ready(() => {
  $('#chk_hold').change(() => {
    const value = $('#chk_hold').val() === 'on';
    enableOnOff(!value);
  });

  $('#chk_onoff').change(() => {
    const value = $('#chk_onoff').val() === 'on';
    enableDashboard(value);
    setHold(value);
    sendOnOff(value);
  });

  $('#chk_enable_log').change(() => {
    const value = $('#chk_enable_log').val() === 'on';
    sendLogEnabled(value);
  });

  $('#btn_reset_voltage').click(() => {
    showPowerMode(0);
    showOutputVoltage(5);
    sendPowermode(0);
    sendOutputVoltage(5, true);
  });

  $('#btn_save_voltage').click(() => {
    const outputVoltage = $('#slider_voltage').val();
    const powerMode = $('input:radio[name=power_mode]:checked').val();
    sendPowermode(powerMode);
    sendOutputVoltage(outputVoltage, true);
  });

  $('#chk_dhcp').change(() => {
    const value = $('#chk_dhcp').val() === 'on';
    enableIpAddrFields(!value);
  });

  $('#btn_save_network').click(() => {
    const wifi = $('input:radio[name=wifi_mode]:checked').val();
    const ssid = $('#tf_ssid').val();
    const passwd = $('#tf_passwd').val();
    const dhcp = $('#chk_dhcp').val() === 'on';
    const ip = $('#tf_ipaddr').val();
    const netmask = $('#tf_netmask').val();
    const gateway = $('#tf_gateway').val();
    const dns = $('#tf_dns').val();

    sendNetwork(wifi, ssid, passwd, dhcp, ip, netmask, gateway, dns);

    $('.ui-pagecontainer').pagecontainer('change', '#dlg_restart', { reverse: false, changeHash: true });
  });

  $('#btn_reset_network').click(() => {
    const wifi = 2;
    const ssid = 'smartpower2';
    const passwd = '12345678';
    const dhcp = false;
    const ip = '192.168.4.1';
    const netmask = '255.255.255.0';
    const gateway = '192.168.4.1';
    const dns = '192.168.4.1';
    showWiFi(wifi);
    showSSID(ssid);
    show(passwd);
    show(dhcp);
    showIPAddr(ip);
    showNetmask(netmask);
    showGateway(gateway);
    showDns(dns);
  });

  $('#power_mode').change(() => {
    const powerMode = $('input:radio[name=power_mode:checked').val();
    sendPowermode(powerMode);
  });

  $('#slider_voltage').change(() => {
    const value = $('#slider_voltage').val();
    sendOutputVoltage(value);
  });

  $('#slider_averaging').change(() => {
    const averaging = $('#slider_averaging').val();
    sendAveraging(averaging);
  });

  $('#dlg_restart_no').click(() => {
    $('.ui-pagecontainer').pagecontainer('change', '#options', {});
  });

  $('#dlg_restart_yes').click(() => {
    $('.ui-pagecontainer').pagecontainer('change', '/restart', {});
  });

  $('#watt_ss').sevenSeg({
    digits: 5,
    value: null,
  });

  $('#amp_ss').sevenSeg({
    digits: 5,
    colorOff: '#003200',
    colorOn: 'Lime',
    value: null,
  });

  $('#volt_ss').sevenSeg({
    digits: 5,
    value: null,
  });

  $('#watth_ss').sevenSeg({
    digits: 5,
    value: null,
  });

  enableDashboard(false);
});
