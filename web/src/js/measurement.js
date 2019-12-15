const RESET_INTERVAL = 60000; // ms

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
