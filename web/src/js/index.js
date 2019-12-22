/* eslint-disable camelcase */
/* eslint-disable no-undef */
const PAGE_HOME = 1;
const PAGE_OPTIONS = 2;
const PAGE_INFO = 3;

let ws;
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
  const uri = `${scheme}${window.location.host}/ws`;
  // return 'ws://192.168.1.204/ws';
  return uri;
}

function getConnectionStatus() {
  try {
    return ws.readyState === WebSocket.OPEN;
  } catch (e) {
    return false;
  }
}

function send(data) {
  try {
    ws.send(data);
    return true;
  } catch (e) {
    return false;
  }
}

//
// Home Page
//
function showPower(value) {
  $('#chk_power').val(value ? 'on' : 'off').flipswitch().flipswitch('refresh');
}

function enableDashboard(enabled = true) {
  // const $obj = $('#power_switch_cont');
  // if (value) {
  //   $obj.addClass('state-on');
  // } else {
  //   $obj.removeClass('state-off');
  // }
  const $obj = $('#dashboard .ss_cont');
  if (enabled) {
    $obj.removeClass('ui-state-disabled');
  } else {
    $obj.addClass('ui-state-disabled');
  }
}

const voltage = new Measurement('V', () => {
  $('#v_ss').sevenSeg({ digits: 5, decimalPlaces: 3, value: voltage.last });
});

const current = new Measurement('A', () => {
  $('#i_ss').sevenSeg({ digits: 5, decimalPlaces: 3, value: current.last });
});

const power = new Measurement('P', () => {
  $('#p_ss').sevenSeg({ digits: 5, decimalPlaces: 3, value: power.last });
});

const watth = new Measurement('W', () => {
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
//
// Options Page
//
function showBootMode(value) {
  const $obj = $('input:radio[name="boot_mode"]');
  $obj.prop('checked', false).checkboxradio().checkboxradio('refresh');
  $obj.filter(`[value="${value}"]`).prop('checked', true).checkboxradio('refresh');
}

function showVoltage(value) {
  $('#voltage').val(value).slider().slider('refresh');
}

function showStoreWh(value) {
  $('#storewh').val(value ? 'on' : 'off').flipswitch().flipswitch('refresh');
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
  $('#chk_dhcp').val(value ? 'on' : 'off').flipswitch().flipswitch('refresh');
}

function enableIPAddr(enabled = true) {
  if (enabled) {
    $('#sta_ipaddr_cont').removeClass('ui-state-disabled');
  } else {
    $('#sta_ipaddr_cont').addClass('ui-state-disabled');
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
//
// Info
//
function showVersion(value) {
  $('#version_info_cont').json2html(value, template, { replace: true });
}

function showNetworkInfo(value) {
  $('#network_info_cont').json2html(value, template, { replace: true });
}

function showSysInfo(value) {
  $('#system_info_cont').json2html(value, template, { replace: true });
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
  return send(JSON.stringify(data));
}

// Home
function setPower(value) {
  const data = { power: value };
  send(JSON.stringify(data));
}
function setWh(value) {
  const data = { wh: value };
  send(JSON.stringify(data));
}

// Options
function setBootMode(value) {
  const data = { set: [{ bootpwr: value }] };
  send(JSON.stringify(data));
}
function setVoltage(value) {
  const data = { set: [{ voltage: value }] };
  send(JSON.stringify(data));
}
function setStoreWh(value) {
  const data = { set: [{ storewh: value ? 1 : 0 }] };
  send(JSON.stringify(data));
}

function setWifi(wifi) {
  const data = {
    set: [{ wifi }],
  };
  send(JSON.stringify(data));
}

function setSta(ssid, passwd, dhcp, ipaddr, netmask, gateway, dns) {
  const data = {
    set: [
      { ssid },
      { passwd },
      { dhcp },
      { ipaddr },
      { netmask },
      { gateway },
      { dns }],
  };
  send(JSON.stringify(data));
}

function setAp(ssid, passwd, ipaddr) {
  const data = {
    set: [
      { ap_ssid: ssid },
      { ap_passwd: passwd },
      { ap_ipaddr: ipaddr },
    ],
  };
  send(JSON.stringify(data));
}

function updateUI(k, v) {
  switch (k) {
    case 'power':
      showPower(v);
      break;
    case 'storewh':
      showStoreWh(v);
      break;
    case 'V':
      voltage.setValue(v);
      break;
    case 'I':
      current.setValue(v);
      break;
    case 'P':
      power.setValue(v);
      break;
    case 'Wh':
      watth.setValue(v);
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
      showDHCP(v);
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
    case 'sysinfo': {
      showSysInfo(v);
      break;
    }
    case 'netinfo': {
      showNetworkInfo(v);
      break;
    }
    case 'version': {
      showVersion(v);
      break;
    }
    default: {
      // eslint-disable-next-line no-console
      console.log('unknown', k);
      break;
    }
  }
}

function parseMessage(data) {
  JSON.parse(data, (k, v) => {
    if (k === '') return;
    updateUI(k, v);
  });
}

// eslint-disable-next-line no-unused-vars
function onload() {
  try {
    const uri = getWSUri();
    ws = new WebSocket(uri);
    ws.onmessage = (msg) => {
      enableUI();
      parseMessage(msg.data);
    };
    ws.onopen = () => {
      const page = getPageIndex($('.ui-page-active').jqmData('title'));
      if (page > 0) setPage(page);
    };
    ws.onclose = () => {
      enableUI(false);
      setTimeout(onload, 1000);
    };
  } catch (e) {
    // eslint-disable-next-line no-console
    console.log(e);
  }
}

$(document).on('pagecontainerbeforeshow', (event, ui) => {
  const name = $(ui.toPage).jqmData('title');
  const page = getPageIndex(name);
  if (page > 0) {
    if (getConnectionStatus() && setPage(page)) {
      $(ui.toPage).removeClass('ui-disabled');
    }
  }
});

$(document).on('pagecontainershow', (event, ui) => {
  const name = $(ui.toPage).jqmData('title');
  $('[data-role="navbar"] a.ui-button-active').removeClass('ui-button-active');
  $('#footer a').each((_index, element) => {
    if ($(element).text() === name) {
      $(element).addClass('ui-button-active');
    }
  });
});

$(document).ready(() => {
  $(() => {
    $('[data-role="navbar"]').navbar();
    $('[data-role="toolbar"]').toolbar();
  });
  // Home
  $('#chk_power').change(() => {
    const value = $('#chk_power').val() === 'on';
    setPower(value);
    enableDashboard(value);
  });
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
  //
  // Options
  //
  $('#slider_averaging').change(() => {
    const value = $('#slider_averaging').val();
    setAveraging(value);
  });
  $('input:radio[name=boot_mode]').change(() => {
    const value = $('input:radio[name=boot_mode]:checked').val();
    setBootMode(value);
  });
  $('#chk_totalwh').change(() => {
    const value = $('#chk_totalwh').val() === 'on';
    setStoreWh(value);
  });
  $('#btn_apply_voltage').click(() => {
    const value = $('#slider_voltage').val();
    setVoltage(parseFloat(value));
  });

  $('#chk_dhcp').change(() => {
    const value = $('#chk_dhcp').val() === 'on';
    enableIPAddr(!value);
  });

  $('#btn_save_wifi_mode').click(() => {
    const wifi = $('input:radio[name=wifi_mode]:checked').val();
    setWifi(wifi);
    $('.ui-pagecontainer').pagecontainer('change', '#dlg_restart', { reverse: false, changeHash: true });
  });

  $('#btn_save_sta').click(() => {
    const ssid = $('#txt_ssid').val();
    const passwd = $('#txt_passwd').val();
    const dhcp = $('#chk_dhcp').val() === 'on';
    const ipaddr = $('#txt_ipaddr').val();
    const netmask = $('#txt_netmask').val();
    const gateway = $('#txt_gateway').val();
    const dns = $('#txt_dns').val();
    setSta(ssid, passwd, dhcp, ipaddr, netmask, gateway, dns);
    $('.ui-pagecontainer').pagecontainer('change', '#dlg_restart', { reverse: false, changeHash: true });
  });

  $('#btn_save_ap').click(() => {
    const ssid = $('#txt_ap_ssid').val();
    const passwd = $('#txt_ap_passwd').val();
    const ipaddr = $('#txt_ap_ipaddr').val();
    setAp(ssid, passwd, ipaddr);
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
    showWiFiMode(wifi);
    showSSID(ssid);
    show(passwd);
    show(dhcp);
    showIPAddr(ip);
    showNetmask(netmask);
    showGateway(gateway);
    showDns(dns);
  });

  $('#dlg_restart_no').click(() => {
    $('.ui-pagecontainer').pagecontainer('change', '#options', {});
  });

  $('#dlg_restart_yes').click(() => {
    $('.ui-pagecontainer').pagecontainer('change', '/restart', {});
  });
});
