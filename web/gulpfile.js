/* eslint-disable no-plusplus */
/* eslint-disable no-console */
const srcFolder = 'src/';
const tmpFolder = 'tmp/';
const headerFolder = 'built/';
const wwwFolder = '../code/data/www';

const del = require('del');
const gulp = require('gulp');
const htmllint = require('gulp-htmllint');
const csslint = require('gulp-csslint');
const gzip = require('gulp-gzip');
const htmlmin = require('gulp-htmlmin');
const inline = require('gulp-inline');
const inlineImages = require('gulp-css-base64');
const favicon = require('gulp-base64-favicon');
const through = require('through2');
const path = require('path');

function cleanTemp() {
  return del([tmpFolder]);
}

gulp.task('webui_clear', () => cleanTemp());

function htmlReport(filepath, issues) {
  issues.forEach((issue) => console.info(`${filepath} at [${issue.line},${issue.column}] ${issue.code}: ${issue.msg}`));
  process.exit(1);
}

function webUIcsslLint() {
  return gulp.src(`${srcFolder}*.css`)
    .pipe(csslint({ ids: false }))
    .pipe(csslint.formatter());
}

gulp.task('webui_csslint', () => webUIcsslLint());

function webUIhtmlLint() {
  return gulp.src(`${srcFolder}*.html`)
    .pipe(htmllint({
      failOnError: true,
      rules: {
        'id-class-style': false,
        'label-req-for': false,
        'line-end-style': false,
      },
    }, htmlReport));
}

gulp.task('webui_htmllint', () => webUIhtmlLint());

function toHeader() {
  return through.obj((src, encoding, cb) => {
    const pathParts = src.path.split(path.sep);
    const srcName = pathParts[pathParts.length - 1];
    const varName = srcName.split('.').join('_');
    const input = src.contents;
    let output = '#include "Arduino.h"\n\n';
    output += `#define ${varName}_len ${input.length}\n\n`;
    output += `static const uint8_t ${varName}[${input.length}] PROGMEM = {`;
    for (let i = 0; i < input.length; ++i) {
      if (i % 24 === 0) { output += '\n'; }
      output += `0x${(`00${input[i].toString(16)}`).slice(-2)}`;
      if (i < input.length - 1) { output += ','; }
    }
    output += '};\n';
    const dest = src.clone();
    dest.path = `${headerFolder}${srcName}.h`;
    dest.contents = Buffer.from(output);
    cb(null, dest);
  });
}

function webUiBuildProgmem() {
  return gulp.src(`${srcFolder}*.html`)
    .pipe(favicon())
    .pipe(inline({
      base: srcFolder,
      js: [],
      css: [inlineImages],
      disabledTypes: ['svg', 'img'],
    }))
    .pipe(htmlmin({
      collapseWhitespace: true,
      removeComments: true,
      minifyCSS: true,
      minifyJS: true,
    }))
    .pipe(gzip())
    .pipe(gulp.dest(tmpFolder))
    .pipe(toHeader())
    .pipe(gulp.dest(headerFolder));
}

function webUiBuildFileSystem() {
  return gulp.src(`${srcFolder}*.html`)
    .pipe(favicon())
    .pipe(inline({
      base: srcFolder,
      js: [],
      css: [inlineImages],
      disabledTypes: ['svg', 'img'],
    }))
    .pipe(htmlmin({
      collapseWhitespace: true,
      removeComments: true,
      minifyCSS: true,
      minifyJS: true,
    }))
    .pipe(gzip())
    .pipe(gulp.dest(wwwFolder));
}
gulp.task('webui_build_progmem', () => webUiBuildProgmem());

gulp.task('webui_build_filesystem', () => webUiBuildFileSystem());

gulp.task('lint', gulp.series('webui_htmllint'));

gulp.task('progmem', gulp.series('webui_build_progmem'));

gulp.task('filesystem', gulp.series('webui_build_filesystem'));
