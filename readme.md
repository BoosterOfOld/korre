# Korre

Korre is a console-based audio player for Mac based on Portaudio.

https://github.com/BoosterOfOld/korre/assets/52963062/1f04e0a6-ac47-4655-a71f-26d2df28bef3

## Supported file types

- __WAV__ - 16 and 24 bit PCM
- __FLAC__ - Supported via the [xiph/flac](https://github.com/xiph/flac) library
- __MP3__ - Supported via the [lieff/minimp3](https://github.com/lieff/minimp3) library
- __TXT__ - Simple playlist (newline-separated paths)

## Features
### Implemented
- Real-time spectrogram with [PFFFT](https://bitbucket.org/jpommier/pffft/src/master/) at its core
- DSP: Impulse response convolution engine that uses the convolution routine from [Armadillo](https://arma.sourceforge.net)
- High-quality audio output based on [Portaudio](https://www.portaudio.com)
- Playlists
- Play queue with Shuffle
- Output device selection

### Coming Soon
- Customizable noise machine module (for when you need to focus and your colleagues won't shut their traps)
- GPU convolution engine
- DSP: Parametric equalizer
- Background audio file decoding allowing you to play/seek a partially loaded file
- Keyboard navigation
- Recording
- Saving recorded or DSP processed audio
- Load files/playlist via console params
- Better integration with Mac OS (open with)

### Coming Later
- More types of visualization
- Custom skins

## How to Build (Mac OS)

### Build ncursesw

The project uses my custom fork of [ggerganov/imtui](https://github.com/ggerganov/imtui) with an added support for UTF-8 (in order to display the box-drawing characters) and as such it needs ncursesw (ncurses with wide char support). Neither the ncurses lib bundled with Mac OS nor the Homebrew cask support that, so ncursesw must be built from source:

Download [ncurses source code](https://ftp.gnu.org/gnu/ncurses/ncurses-6.4.tar.gz) and build using:

```
./configure --prefix=/usr/local \\n  --without-cxx --without-cxx-binding --without-ada --without-progs --without-curses-h \\n  --with-shared --without-debug \\n  --enable-widec --enable-const --enable-ext-colors --enable-sigwinch --enable-wgetch-events \\n&& make
```
```
sudo make install
```

### Build FLAC

```
git clone https://github.com/xiph/flac.git
cd flac
cmake . -DWITH_OGG=OFF -DWITH_INSTALL_MANPAGES=OFF
make
make test
sudo make install
```

### Install Armadillo
```
brew install armadillo
```

### Install Portaudio
```
brew install portaudio
```

### Build
```
cmake .
make
```
