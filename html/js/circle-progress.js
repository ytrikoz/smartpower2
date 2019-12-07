/* eslint-disable */
(function ($) {
	function CircleProgress(config) {
		this.init(config);
	}

	CircleProgress.prototype = {
		value: 0.0,
		size: 100.0,
		startAngle: -Math.PI,
		endAngle: Math.PI,
		thickness: 'auto',
		fill: {
			gradient: ['#3aeabb', '#fdd250'],
		},
		emptyFill: 'rgba(0, 0, 0, .1)',
		animation: {
			duration: 1200,
			easing: 'circleProgressEasing',
		},
		animationStartValue: 0.0,
		reverse: false,
		lineCap: 'butt',

		constructor: CircleProgress,

		el: null,
		canvas: null,
		ctx: null,
		radius: 0.0,
		arcFill: null,
		lastFrameValue: 0.0,

		init(config) {
			$.extend(this, config);
			this.radius = this.size / 2;
			this.initWidget();
			this.initFill();
			this.draw();
		},

		initWidget() {
			const canvas = this.canvas = this.canvas || $('<canvas>').prependTo(this.el)[0];
			canvas.width = this.size;
			canvas.height = this.size;
			this.ctx = canvas.getContext('2d');
		},

		initFill() {
			const self = this;
			const { fill } = this;
			const { ctx } = this;
			const { size } = this;

			function setImageFill() {
				const bg = $('<canvas>')[0];
				bg.width = self.size;
				bg.height = self.size;
				bg.getContext('2d').drawImage(img, 0, 0, size, size);
				self.arcFill = self.ctx.createPattern(bg, 'no-repeat');
				self.drawFrame(self.lastFrameValue);
			}

			if (!fill) throw Error('The fill is not specified!');

			if (fill.color) this.arcFill = fill.color;

			if (fill.gradient) {
				const gr = fill.gradient;
				if (gr.length === 1) {
					this.arcFill = gr[0];
				} else if (gr.length > 1) {
					// gradient direction angle; 0 by default
					const ga = fill.gradientAngle || 0;
					const gd = fill.gradientDirection || [
						size / 2 * (1 - Math.cos(ga)), // x0
						size / 2 * (1 + Math.sin(ga)), // y0
						size / 2 * (1 + Math.cos(ga)), // x1
						size / 2 * (1 - Math.sin(ga)), // y1
					];
					const lg = ctx.createLinearGradient.apply(ctx, gd);

					for (let i = 0; i < gr.length; i += 1) {
						color = gr[i];
						pos = i / (gr.length - 1);

						if ($.isArray(color)) {
							pos = color[1];
							color = color[0];
						}

						lg.addColorStop(pos, color);
					}

					this.arcFill = lg;
				}
			}

			if (fill.image) {
				let img;

				if (fill.image instanceof Image) {
					img = fill.image;
				} else {
					img = new Image();
					img.src = fill.image;
				}

				if (img.complete) { setImageFill(); } else { img.onload = setImageFill; }
			}
		},

		draw() {
			if (this.animation) { this.drawAnimated(this.value); } else { this.drawFrame(this.value); }
		},

		drawFrame(v) {
			this.lastFrameValue = v;
			this.ctx.clearRect(0, 0, this.size, this.size);
			this.drawEmptyArc(v);
			this.drawArc(v);
		},

		drawArc(v) {
			const { ctx } = this;
			const r = this.radius;
			const t = this.getThickness();
			const a = this.startAngle;
			const ea = this.endAngle;

			ctx.save();
			ctx.beginPath();

			if (!this.reverse) {
				ctx.arc(r, r, r - t / 2, a, a + Math.PI * v);
			} else {
				ctx.arc(r, r, r - t / 2, a - Math.PI * v, a);
			}

			ctx.lineWidth = t;
			ctx.lineCap = this.lineCap;
			ctx.strokeStyle = this.arcFill;
			ctx.stroke();
			ctx.restore();
		},

		drawEmptyArc(v) {
			const { ctx } = this;
			const r = this.radius;
			const t = this.getThickness();
			const a = this.startAngle;
			const ea = this.startAngle;

			if (v < 1) {
				ctx.save();
				ctx.beginPath();

				if (v <= 0) {
					ctx.arc(r, r, r - t / 2, 0, Math.PI);
				} else if (!this.reverse) {
					ctx.arc(r, r, r - t / 2, a + Math.PI * v, a);
				} else {
					ctx.arc(r, r, r - t / 2, a, a - Math.PI * v);
				}

				ctx.lineWidth = t;
				ctx.strokeStyle = this.emptyFill;
				ctx.stroke();
				ctx.restore();
			}
		},

		drawAnimated(v) {
			const self = this;
			const { el } = this;
			const canvas = $(this.canvas);

			canvas.stop(true, false);
			el.trigger('circle-animation-start');

			canvas
				.css({ animationProgress: 0 })
				.animate({ animationProgress: 1 }, $.extend({}, this.animation, {
					step(animationProgress) {
						const stepValue = self.animationStartValue
							* (1 - animationProgress) + v * animationProgress;
						self.drawFrame(stepValue);
						el.trigger('circle-animation-progress', [animationProgress, stepValue]);
					},
				}))
				.promise()
				.always(() => {
					el.trigger('circle-animation-end');
				});
		},

		getThickness() {
			return $.isNumeric(this.thickness) ? this.thickness : this.size / 14;
		},

		getValue() {
			return this.value;
		},

		setValue(newValue) {
			if (this.animation) { this.animationStartValue = this.lastFrameValue; }
			this.value = newValue;
			this.draw();
		},
	};

	$.circleProgress = {
		defaults: CircleProgress.prototype,
	};

	$.easing.circleProgressEasing = function (x, t, b, c, d) {
		if ((t /= d / 2) < 1) { return c / 2 * t * t * t + b; }
		return c / 2 * ((t -= 2) * t * t + 2) + b;
	};

	$.fn.circleProgress = function (configOrCommand, commandArgument) {
		const dataName = 'circle-progress';
		const firstInstance = this.data(dataName);

		if (configOrCommand === 'widget') {
			if (!firstInstance) {
				throw Error('Calling "widget" method on not initialized instance is forbidden');
			}
			return firstInstance.canvas;
		}

		if (configOrCommand === 'value') {
			if (!firstInstance) { throw Error('Calling "value" method on not initialized instance is forbidden'); }
			if (typeof commandArgument === 'undefined') {
				return firstInstance.getValue();
			}
			const newValue = arguments[1];
			return this.each(function () {
				$(this).data(dataName).setValue(newValue);
			});
		}

		return this.each(function () {
			const el = $(this);
			let instance = el.data(dataName);
			let config = $.isPlainObject(configOrCommand) ? configOrCommand : {};

			if (instance) {
				instance.init(config);
			} else {
				const initialConfig = $.extend({}, el.data());
				if (typeof initialConfig.fill === 'string') { initialConfig.fill = JSON.parse(initialConfig.fill); }
				if (typeof initialConfig.animation === 'string') { initialConfig.animation = JSON.parse(initialConfig.animation); }
				config = $.extend(initialConfig, config);
				config.el = el;
				instance = new CircleProgress(config);
				el.data(dataName, instance);
			}
		});
	};
}(jQuery));
