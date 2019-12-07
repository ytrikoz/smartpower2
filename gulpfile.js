const fs = require('fs');
const del = require('del');
const gulp = require('gulp');

const htmllint = require('gulp-htmllint');
const csslint = require('gulp-csslint');

const gzip = require('gulp-gzip');
const htmlmin = require('gulp-htmlmin');

const inline = require('gulp-inline');
const inlineImages = require('gulp-css-base64');
const favicon = require('gulp-base64-favicon');
const crass = require('gulp-crass');

const htmlFolder = 'html/';
const dataFolder = 'data/';
const tmpFolder = 'tmp/';
const destFolder = 'include/Static/';


var htmllintReporter = function (filepath, issues) {
    if (issues.length > 0) {
        issues.forEach(function (issue) {
            console.info(
                '[gulp-htmllint] ' +
                filepath + ' [' +
                issue.line + ',' +
                issue.column + ']: ' +
                '(' + issue.code + ') ' +
                issue.msg
            );
        });
        process.exitCode = 1;
    }
};

var WebUI_clean = function () {
    return del([tmpFolder, destFolder + 'index.html.gz.h',]);
};

var WebUI_inline = function () {
    return gulp.src(htmlFolder + '*.html').
        pipe(favicon()).
        pipe(inline({
            base: htmlFolder,
            js: [],
            css: [inlineImages],
            disabledTypes: ['svg', 'img']
        })).
        pipe(htmlmin({
            collapseWhitespace: true,
            removeComments: true,
            minifyCSS: true,
            minifyJS: true
        })).
        pipe(gzip()).
        pipe(gulp.dest(tmpFolder));
};

var WebUI_embeded = function () {
    var src = tmpFolder + 'index.html.gz';
    var dest = destFolder + 'index.html.gz.h';
    var wstream = fs.createWriteStream(dest);
    wstream.on('error', function (err) {
        console.log(err);
    });
    var data = fs.readFileSync(src);
    wstream.write('#define index_html_gz_len ' + data.length + '\r\n');
    wstream.write('static const char index_html_gz[] PROGMEM = {')
    for (i = 0; i < data.length; i++) {
        if (i % 255 == 0) wstream.write("\n");
        wstream.write('0x' + ('00' + data[i].toString(16)).slice(-2));
        if (i < data.length - 1) wstream.write(',');
    }
    wstream.write('\r\n};')
    wstream.end();
    return del([tmpFolder]);
};

gulp.task('webui_htmllint', function () {
    return gulp.src(htmlFolder + '*.html').
    pipe(htmllint({
        'failOnError': true,
        'rules': {
            'id-class-style': false,
            'label-req-for': false,
            'line-end-style': false,
        }
    }, htmllintReporter));
});

gulp.task('webui_csslint', function () {
    return gulp.src(htmlFolder + '*.css').
        pipe(csslint({ ids: false })).
        pipe(csslint.formatter());
});

gulp.task('webui_clean', function () {
    return WebUI_clean();
});

gulp.task('webui_inline', function () {
    return WebUI_inline();
});

gulp.task('webui_embeded', async function() {
    return WebUI_embeded();
});

gulp.task('build_webui', gulp.series('webui_htmllint', 'webui_csslint', 'webui_clean', 'webui_inline', 'webui_embeded'));

gulp.task('default', gulp.series('build_webui'));