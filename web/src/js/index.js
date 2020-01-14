/* eslint-disable camelcase */
/* eslint-disable no-undef */
const PAGE_HOME = 1;
const PAGE_OPTIONS = 2;
const PAGE_INFO = 3;
const DEBUG_WS = '192.168.1.204';

let ws;
let conn = false;

const RESET_INTERVAL = 60000;

class Measurement {
  constructor(name, onChange) {
    this.name = name;
    this.onChange = onChange;
    this.lastStatUpdate = $.now();
    this.reset();
  }

  setValue(newValue) {
    if ($.now() - RESET_INTERVAL > this.lastStatUpdate) {
      this.reset();
    }
    let changes = false;
    if ((typeof this.lastValue === 'undefined') || (this.curValue !== newValue)) {
      this.lastValue = newValue;
      changes = true;
    }
    if ((typeof this.minValue === 'undefined') || ((this.minValue > newValue) || (this.newValue !== 0))) {
      this.minValue = newValue;
      changes = true;
    }
    if ((typeof this.maxValue === 'undefined') || (this.maxValue < newValue)) {
      this.maxValue = newValue;
      changes = true;
    }
    this.samplesCounter += 1;
    if (changes) {
      this.onChange(this);
    }
  }

  get counter() {
    return this.samplesCounter;
  }

  get last() {
    return this.lastValue;
  }

  get max() {
    return this.maxValue;
  }

  get min() {
    return this.minValue;
  }

  get stat() {
    return `${this.name} - ${this.counter} from ${this.min} to ${this.max}`;
  }

  reset() {
    this.maxValue = undefined;
    this.minValue = undefined;
    this.lastValue = undefined;
    this.samplesCounter = 0;
  }
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

function getWSUri() {
  const scheme = window.location.protocol === 'https:' ? 'wss://' : 'ws://';
  let host = window.location.hostname;
  if (host === '127.0.0.1') host = DEBUG_WS;
  const uri = `${scheme}${host}/ws`;
  return uri;
}

function getConnectionStatus() {
  try {
    return ws.readyState === WebSocket.OPEN;
  } catch (e) {
    return false;
  }
}

function sendJsonObject(obj) {
  try {
    return ws.send(JSON.stringify(obj));
  } catch (e) {
    // eslint-disable-next-line no-console
    console.error(e);
    return false;
  }
}

function askRestart() {
  $('.ui-pagecontainer').pagecontainer('change', '#restart', { reverse: false, changeHash: true });
}

function saveConfig() {
  const data = { config: 'save' };
  sendJsonObject(data);
}

function enableDashboard(enabled = true) {
  const $obj = $('#dashboard .ss_cont');
  if (enabled) {
    $obj.removeClass('ui-state-disabled');
  } else {
    $obj.addClass('ui-state-disabled');
  }
}

function setStoreWh(value) {
  const data = { set: [{ store_wh: value ? 1 : 0 }] };
  sendJsonObject(data);
  saveConfig();
}

function setBacklight(value) {
  const data = { set: [{ backlight: value ? 1 : 0 }] };
  sendJsonObject(data);
  saveConfig();
}

function setPower(value) {
  const data = { power: value ? 1 : 0 };
  sendJsonObject(data);
}

function onChangeStoreWh() {
  const value = $('#flipswitch_store_wh').val() === 'on';
  setStoreWh(value);
}

function onChangeBacklight() {
  const value = $('#flipswitch_backlight').val() === 'on';
  setBacklight(value);
}

function onChangePower() {
  const value = $('#flipswitch_power').val() === 'on';
  setPower(value);
  enableDashboard(value);
}

function showStoreWh(value) {
  $('#flipswitch_store_wh').unbind('change', onChangeStoreWh);
  $('#flipswitch_store_wh').val(value ? 'on' : 'off').flipswitch().flipswitch('refresh');
  $('#flipswitch_store_wh').bind('change', onChangeStoreWh);
}

function showBacklight(value) {
  $('#flipswitch_backlight').unbind('change', onChangeBacklight);
  $('#flipswitch_backlight').val(value ? 'on' : 'off').flipswitch().flipswitch('refresh');
  $('#flipswitch_backlight').bind('change', onChangeBacklight);
}

function showPower(value) {
  $('#flipswitch_power').unbind('change', onChangePower);
  $('#flipswitch_power').val(value ? 'on' : 'off').flipswitch().flipswitch('refresh');
  $('#flipswitch_power').bind('change', onChangePower);
}


const voltage = new Measurement('V', () => {
  $('#v_ss').sevenSeg({ digits: 5, decimalPlaces: 3, value: voltage.last });
});

const current = new Measurement('A', () => {
  $('#i_ss').sevenSeg({ digits: 5, decimalPlaces: 3, value: current.last });
});

const power = new Measurement('W', () => {
  $('#p_ss').sevenSeg({ digits: 5, decimalPlaces: 3, value: power.last });
});

const watth = new Measurement('Wh', () => {
  if (watth.last < 10) {
    $('#wh_ss').sevenSeg({ digits: 5, decimalPlaces: 3 });
  } else if (watth.last < 100) {
    $('#wh_ss').sevenSeg({ digits: 5, decimalPlaces: 2 });
  } else if (watth.last < 1000) {
    $('#wh_ss').sevenSeg({ digits: 5, decimalPlaces: 1 });
  } else {
    $('#wh_ss').sevenSeg({ digits: 5, decimalPlaces: 0 });
  }
  $('#wh_ss').sevenSeg({ value: watth.last });
});

function showBootMode(value) {
  const $obj = $('input:radio[name="boot_mode"]');
  $obj.prop('checked', false).checkboxradio().checkboxradio('refresh');
  $obj.filter(`[value="${value}"]`).prop('checked', true).checkboxradio('refresh');
}

function showVoltage(value) {
  $('#slider_voltage').val(value).slider().slider('refresh');
}

function showTWP(value) {
  $('#slider_twp').val(value).slider().slider('refresh');
}

function showSyslog(value) {
  $('#txt_syslog').val(value).textinput().textinput('refresh');
}

//
// Wifi Mode
//
function showWiFi(value) {
  const $wifiMode = $('input:radio[name="wifi_mode"]');
  $wifiMode.prop('checked', false).checkboxradio().checkboxradio('refresh');
  $wifiMode.filter(`[value="${value}"]`).prop('checked', true).checkboxradio('refresh');
}
//
// STA
//
function showSSID(value) {
  $('#txt_ssid').val(value).textinput().textinput('refresh');
}

function showPasswd(value) {
  $('#txt_passwd').val(value).textinput().textinput('refresh');
}

function showDHCP(value) {
  $('#flipswitch_dhcp').val(value ? 'on' : 'off').flipswitch().flipswitch('refresh');
}

function enableIPAddr(enabled = true) {
  if (enabled) {
    $('#ipaddr_cont').removeClass('ui-state-disabled');
  } else {
    $('#ipaddr_cont').addClass('ui-state-disabled');
  }
}

function showIPAddr(value) {
  $('#txt_ipaddr').val(value).textinput().textinput('refresh');
}

function showNetmask(value) {
  $('#txt_netmask').val(value).textinput().textinput('refresh');
}

function showGateway(value) {
  $('#txt_gateway').val(value).textinput().textinput('refresh');
}

function showDns(value) {
  $('#txt_dns').val(value).textinput().textinput('refresh');
}
//
// AP
//
function showAP_SSID(value) {
  $('#txt_ap_ssid').val(value).textinput().textinput('refresh');
}

function showAP_Passwd(value) {
  $('#txt_ap_passwd').val(value).textinput().textinput('refresh');
}

function showAP_IPAddr(value) {
  $('#txt_ap_ipaddr').val(value).textinput().textinput('refresh');
}

function enableUI(value = true) {
  if (value) {
    $('.ui-pagecontainer').removeClass('ui-disabled');
  } else {
    $('.ui-pagecontainer').addClass('ui-disabled');
  }
}

function setPage(value) {
  const data = { page: value };
  return sendJsonObject(data);
}

function restart() {
  const data = { system: 'restart' };
  sendJsonObject(data);
}

function setWh(value) {
  const data = { wh: value };
  sendJsonObject(data);
}

// Options
function setBootMode(value) {
  const data = { set: [{ bootpwr: value }] };
  sendJsonObject(data);
  saveConfig();
}

function setVoltage(value) {
  const data = { set: [{ voltage: value }] };
  sendJsonObject(data);
  saveConfig();
}

function setSyslog(value) {
  const data = { set: [{ syslog: value }] };
  sendJsonObject(data);
  saveConfig();
}

function setWifi(wifi, twp) {
  const data = {
    set: [{ wifi, twp }],
  };
  sendJsonObject(data);
  saveConfig();
  askRestart();
}

function setSta(ssid, passwd, dhcp, ipaddr, netmask, gateway, dns) {
  const data = {
    set: [
      { ssid },
      { dhcp: dhcp ? 1 : 0 },
    ],
  };
  if (!dhcp) {
    data.set.push(
      { ipaddr },
      { netmask },
      { gateway },
      { dns },
    );
  }
  if (passwd !== '') {
    data.set.push({ passwd });
  }
  sendJsonObject(data);
  saveConfig();
}

function setAp(ssid, passwd, ipaddr) {
  const data = {
    set: [
      { ap_ssid: ssid },
      { ap_ipaddr: ipaddr },
    ],
  };
  if (passwd !== '') {
    data.set.push({ ap_passwd: passwd });
  }
  sendJsonObject(data);
  saveConfig();
}

function updateUI(k, v) {
  switch (k) {
    case 'power': {
      const value = parseInt(v, 10);
      showPower(value);
      enableDashboard(value);
      break;
    }
    case 'store_wh':
      showStoreWh(parseInt(v, 10));
      break;
    case 'v':
      voltage.setValue(parseFloat(v));
      break;
    case 'i':
      current.setValue(parseFloat(v));
      break;
    case 'p':
      power.setValue(parseFloat(v));
      break;
    case 'wh':
      watth.setValue(parseFloat(v));
      break;
    case 'bootpwr':
      showBootMode(parseInt(v, 10));
      break;
    case 'voltage':
      showVoltage(parseFloat(v));
      break;
    case 'wifi':
      showWiFi(parseInt(v, 10));
      break;
    case 'twp':
      showTWP(parseInt(v, 10));
      break;
    case 'ssid':
      showSSID(v);
      break;
    case 'ap_ssid':
      showAP_SSID(v);
      break;
    case 'passwd':
      showPasswd(v);
      break;
    case 'ap_passwd':
      showAP_Passwd(v);
      break;
    case 'dhcp':
      showDHCP(parseInt(v, 10));
      break;
    case 'ipaddr':
      showIPAddr(v);
      break;
    case 'ap_ipaddr':
      showAP_IPAddr(v);
      break;
    case 'gateway':
      showGateway(v);
      break;
    case 'netmask':
      showNetmask(v);
      break;
    case 'dns':
      showDns(v);
      break;
    case 'backlight':
      showBacklight(parseInt(v, 10));
      break;
    case 'syslog':
      showSyslog(v);
      break;
    default:
      // eslint-disable-next-line no-console
      if (JSON.stringify(v) === JSON.stringify({})) break;
      console.log('unknown', k, v);
  }
}

function parseMessage(data) {
  JSON.parse(data, (k, v) => {
    if (k === '') { return undefined; }
    return updateUI(k, v);
  });
}

// eslint-disable-next-line no-unused-vars
function onload() {
  try {
    const uri = getWSUri();
    ws = new WebSocket(uri);
    ws.onmessage = (msg) => {
      conn = true;
      enableUI();
      parseMessage(msg.data);
    };
    ws.onopen = () => {
      conn = true;
      enableUI();
      const page = getPageIndex($('.ui-page-active').jqmData('title'));
      if (page > 0) setPage(page);
    };
    ws.onclose = () => {
      if (conn) {
        enableUI(false);
      }
      conn = false;
      setTimeout(onload, 1000);
    };
  } catch (e) {
    // eslint-disable-next-line no-console
    console.log(e);
  }
}

$(() => {
  $('[data-role="header"], [data-role="footer"]').toolbar();
  $('[data-role="navbar"]').navbar();
});

$(document).on('pagecontainerbeforeshow', (event, ui) => {
  const page = getPageIndex($(ui.toPage).jqmData('title'));
  if (page > 0) {
    if (getConnectionStatus() && setPage(page)) {
      $(ui.toPage).removeClass('ui-disabled');
    }
  }
});

$(document).on('pagecontainershow', (event, ui) => {
  const name = $(ui.toPage).jqmData('title');
  const title = `SmartPower2: ${name}`;
  $('[data-role="header"] h1').text(title);
  $('[data-role="navbar"] a.ui-button-active').removeClass('ui-button-active');
  $('[data-role="navbar"] a').each((index, element) => {
    if ($(element).text() === name) {
      $(element).addClass('ui-button-active');
    }
  });
});

$(document).ready(() => {
  $('#flipswitch_power').bind('change', onChangePower);
  $('#flipswitch_store_wh').bind('change', onChangeStoreWh);
  $('#flipswitch_backlight').bind('change', onChangeBacklight);

  $('#v_ss').sevenSeg({
    digits: 5,
    value: null,
  });

  $('#p_ss').sevenSeg({
    digits: 5,
    value: null,
  });

  $('#i_ss').sevenSeg({
    digits: 5,
    colorOff: '#003200',
    colorOn: 'Lime',
    value: null,
  });

  $('#wh_ss').sevenSeg({
    digits: 5,
    value: null,
  });

  $('#btn_wh_zero').click(() => {
    setWh(0);
  });

  $('#slider_averaging').change(() => {
    const value = $('#slider_averaging').val();
    setAveraging(value);
  });

  $('input:radio[name=boot_mode]').change(() => {
    const value = $('input:radio[name=boot_mode]:checked').val();
    setBootMode(value);
  });

  $('#txt_syslog').change(() => {
    const value = $('#txt_syslog').val();
    setSyslog(value);
  });

  $('#btn_apply_voltage').click(() => {
    const value = $('#slider_voltage').val();
    setVoltage(parseFloat(value));
  });

  $('#flipswitch_dhcp').change(() => {
    const value = $('#flipswitch_dhcp').val() === 'on';
    enableIPAddr(!value);
  });

  $('#btn_save_wifi').click(() => {
    const wifi = $('input:radio[name=wifi_mode]:checked').val();
    const twp = $('#slider_twp').val();
    setWifi(wifi, twp);
  });

  $('#btn_save_sta').click(() => {
    const ssid = $('#txt_ssid').val();
    const passwd = $('#txt_passwd').val();
    const dhcp = $('#flipswitch_dhcp').val() === 'on';
    const ipaddr = $('#txt_ipaddr').val();
    const netmask = $('#txt_netmask').val();
    const gateway = $('#txt_gateway').val();
    const dns = $('#txt_dns').val();
    setSta(ssid, passwd, dhcp, ipaddr, netmask, gateway, dns);
  });

  $('#btn_save_ap').click(() => {
    const ssid = $('#txt_ap_ssid').val();
    const passwd = $('#txt_ap_passwd').val();
    const ipaddr = $('#txt_ap_ipaddr').val();
    setAp(ssid, passwd, ipaddr);
  });

  $('#restart-button').click(() => {
    restart();
  });

  $('#version_cont').on('collapsibleexpand', (event, ui) => {
    $.getJSON('version', (data) => {
      const items = [];
      $.each(data[0], (key, val) => {
        items.push(`<li>${key}: ${val}</li>`);
      });
      $('#version_div').replaceWith($('<ul/>', { class: 'key-value-list', html: items.join('') }));
    });
  });

  $('#system_cont').on('collapsibleexpand', (event, ui) => {
    $.getJSON('system', (data) => {
      const items = [];
      $.each(data[0], (key, val) => {
        items.push(`<li>${key}:${val}</li>`);
      });
      $('#system_div').replaceWith($('<ul/>', { class: 'key-value-list', html: items.join('') }));
    });
  });

  $('#network_cont').on('collapsibleexpand', (event, ui) => {
    $.getJSON('network', (data) => {
      const items = [];
      $.each(data[0], (key, val) => {
        items.push(`<li>${key}:${val}</li>`);
      });
      $('#network_div').replaceWith($('<ul/>', { class: 'key-value-list', html: items.join('') }));
    });
  });
});
