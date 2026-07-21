<div align="center">

⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️

# VIBECODE WARNING

This is made by ChatGPT Codex. I made it for my own personal use and for people who were interested in atleast somewhat working soundux.

The webview is changed to Qt WebEngine and native wayland works better now.
Application can stream right to microphone, without having to deal with audio devices each time
The app automatically refreshes the applications and can remember them

⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️

</div>

<hr/>

<div align="center">
  <p>
    Read the documentation in:
    <br>
    <a href="https://github.com/Soundux/Soundux/blob/master/README.md">[English]</a>
    <a href="https://github.com/Soundux/Soundux/blob/master/i18n/LISEZMOI.md">[French]</a>
    <a href="https://github.com/Soundux/Soundux/blob/master/i18n/ПРОЧТИМЕНЯ.md">[Russian]</a>
    <br><br><br>
    <img src="assets/logo.gif" height="200"/>
    <br>
    <h6>A Linux soundboard 🔊</h6>
    <br>
        <a href="https://github.com/7pxvr/Soundux-mod/releases">
      <img src="https://img.shields.io/github/release/7pxvr/Soundux-mod.svg?style=flat-square" alt="Latest Stable Release" />
    </a>
    <br>
    <a href="https://github.com/7pxvr/Soundux-mod/stargazers">
      <img src="https://img.shields.io/github/stars/7pxvr/Soundux-mod?style=flat-square" alt="GitHub Repo stars">
    </a>
    <a href="https://github.com/7pxvr/Soundux-mod/issues">
      <img src="https://img.shields.io/github/issues/7pxvr/Soundux-mod?style=flat-square" alt="GitHub issues">
    </a>
    <a href="https://github.com/7pxvr/Soundux-mod/pulls">
      <img src="https://img.shields.io/github/issues-pr-raw/7pxvr/Soundux-mod?label=pulls&style=flat-square" alt="GitHub pull requests">
    </a>
    <br>
    <a href="https://github.com/7pxvr/Soundux-mod/blob/master/LICENSE">
      <img src="https://img.shields.io/github/license/7pxvr/Soundux-mod.svg?style=flat-square" alt="License" />
    </a>
    <a href="https://discord.gg/4HwSGN4Ec2">
      <img src="https://img.shields.io/discord/697348809591750706?label=discord&style=flat-square" alt="Discord" />
    </a>
    <a href="https://matrix.to/#/!XlIlRgKzoRavKnurkt:matrix.org">
      <img src="https://img.shields.io/badge/chat-matrix%20space-blue?style=flat-square" alt="Matrix" />
    </a>
    <br>
    <a href="https://github.com/7pxvr/Soundux-mod/actions/workflows/compile_arch.yml">
      <img src="https://img.shields.io/github/actions/workflow/status/7pxvr/Soundux-mod/compile_arch.yml?branch=master&label=pacman%20package&style=flat-square" alt="Pacman package build" />
    </a>
    <hr>
    <a href="https://discord.com/invite/4HwSGN4Ec2">
      <img src="https://invidget.switchblade.xyz/4HwSGN4Ec2" alt="Discord Invite"/>
    </a>
    <a href="https://hosted.weblate.org/engage/soundux/">
      <img src="https://hosted.weblate.org/widgets/soundux/-/frontend/multi-green.svg" alt="Translation status" />
    </a>
  </p>
</div>

# 👀 Preview
| ![Dark Interface](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/home-dark.png)                   | ![Light Interface](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/home-light.png)                   |
| -------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------- |
| ![Settings Dark](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/settings-dark.png)                | ![Settings Light](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/settings-light.png)                |
| ![Search Dark](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/search-dark.png)                    | ![Search Light](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/search-light.png)                    |
| ![Application Passthrough](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/pass-through-dark.png)  | ![Application Passthrough](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/pass-through-light.png)   |
| ![Seek/Pause/Stop Dark](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/multiple-playing-dark.png) | ![Seek/Pause/Stop Light](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/multiple-playing-light.png) |
| ![Grid View Dark](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/grid-view-dark.png)              | ![Grid View Light](https://raw.githubusercontent.com/Soundux/screenshots/screenshots/grid-view-light.png)              |

# 👋 Introduction
Soundux-mod is a Linux soundboard based on Soundux.
With Soundux-mod you can play audio to a specific application or stream it directly to your microphone.

# 🏃 Runtime Dependencies
These are required to run the program

## 🐧 Linux
Please refer to your distro instructions on how to install
- [pulseaudio](https://gitlab.freedesktop.org/pulseaudio/pulseaudio) / [pipewire](https://pipewire.org/) >= 0.3.26
- Qt 6 WebEngine and Qt 6 WebChannel
- X11 libraries for legacy X11 support
- Libwnck3 (optional, X11 icon support)
- libappindicator3 (optional, tray support where available)
- [yt-dlp](https://github.com/yt-dlp/yt-dlp) or [youtube-dl](https://youtube-dl.org/) & [ffmpeg](https://www.ffmpeg.org/) (optional, for downloader support)

# 📥 Installation

## 🐧 Linux

### <img src="https://www.vectorlogo.zone/logos/archlinux/archlinux-icon.svg" height="20"/> Arch Linux and derivatives
Download the `soundux-mod-*.pkg.tar.zst` package from the [latest GitHub release](https://github.com/7pxvr/Soundux-mod/releases/latest), then install it with pacman:
```sh
sudo pacman -U soundux-mod-*.pkg.tar.zst
```

### <img src="https://www.vectorlogo.zone/logos/linuxfoundation/linuxfoundation-icon.svg" height="20" /> Other Linux distributions
Prebuilt release packages currently target pacman/Arch only. Build from source if you use another distribution.

# 🔨 Compilation

## 🔗 Build Dependencies

### 🐧 Linux
- Qt 6 Base, WebEngine, and WebChannel development files
- PulseAudio development headers
- PipeWire development headers
- X11 client-side development headers
- GTK3 development headers
- libappindicator3 development headers
- OpenSSL development headers
- G++ >= 9
  - Some distros still have G++ versions < 9 in their repos, using them will result in a build failure (for more information refer to [#71](https://github.com/Soundux/Soundux/issues/71)).

#### <img src="https://www.vectorlogo.zone/logos/debian/debian-icon.svg" height="20"/> Debian / <img src="https://www.vectorlogo.zone/logos/ubuntu/ubuntu-icon.svg" height="20"/> Ubuntu and derivatives
```sh
sudo apt install git build-essential cmake qt6-base-dev qt6-base-dev-tools qt6-webengine-dev qt6-webengine-dev-tools qt6-webchannel-dev libx11-dev libxi-dev libgtk-3-dev libwnck-3-dev libappindicator3-dev libssl-dev libpulse-dev libpipewire-0.3-dev
```
> If you're on Ubuntu 20.04 or lower you might have to add the PipeWire PPA:
> `sudo add-apt-repository ppa:pipewire-debian/pipewire-upstream`
#### <img src="https://www.vectorlogo.zone/logos/getfedora/getfedora-icon.svg" height="20"> Fedora and derivatives
```sh
sudo dnf install git cmake gcc-c++ qt6-qtbase-devel qt6-qtwebengine-devel qt6-qtwebchannel-devel libXi-devel gtk3-devel libwnck3-devel libappindicator-gtk3-devel openssl-devel pulseaudio-libs-devel pipewire-devel
```

## 👷 Build
Clone the repository
```sh
git clone https://github.com/7pxvr/Soundux-mod.git
cd Soundux-mod
git submodule update --init --recursive
```
Create a build folder and start compilation
```sh
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
To start the program
```sh
./soundux
```

## 🖥️ Install

### 🐧 Linux
```sh
sudo make install
```

# 📝 Why _Soundux_?

The project started as a **Sound**board for Lin**ux**

# 🗒️ License
The code is licensed under [GPLv3](LICENSE)

# ✍️ Contributing
The contribution guidelines can be found [here](CONTRIBUTING.md), please check them out if you're planning to contribute!

# ✨ Contributors

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="https://github.com/Curve"><img src="https://avatars.githubusercontent.com/u/37805707?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Noah</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3ACurve" title="Bug reports">🐛</a> <a href="#business-Curve" title="Business development">💼</a> <a href="https://github.com/Soundux/Soundux/commits?author=Curve" title="Code">💻</a> <a href="#design-Curve" title="Design">🎨</a> <a href="#ideas-Curve" title="Ideas, Planning, & Feedback">🤔</a> <a href="#infra-Curve" title="Infrastructure (Hosting, Build-Tools, etc)">🚇</a> <a href="#maintenance-Curve" title="Maintenance">🚧</a> <a href="#platform-Curve" title="Packaging/porting to new platform">📦</a> <a href="#projectManagement-Curve" title="Project Management">📆</a> <a href="#question-Curve" title="Answering Questions">💬</a> <a href="https://github.com/Soundux/Soundux/pulls?q=is%3Apr+reviewed-by%3ACurve" title="Reviewed Pull Requests">👀</a> <a href="https://github.com/Soundux/Soundux/commits?author=Curve" title="Tests">⚠️</a></td>
    <td align="center"><a href="https://github.com/D3SOX"><img src="https://avatars.githubusercontent.com/u/24937357?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Nico</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3AD3SOX" title="Bug reports">🐛</a> <a href="#business-D3SOX" title="Business development">💼</a> <a href="https://github.com/Soundux/Soundux/commits?author=D3SOX" title="Code">💻</a> <a href="#design-D3SOX" title="Design">🎨</a> <a href="#ideas-D3SOX" title="Ideas, Planning, & Feedback">🤔</a> <a href="#infra-D3SOX" title="Infrastructure (Hosting, Build-Tools, etc)">🚇</a> <a href="#maintenance-D3SOX" title="Maintenance">🚧</a> <a href="#platform-D3SOX" title="Packaging/porting to new platform">📦</a> <a href="#projectManagement-D3SOX" title="Project Management">📆</a> <a href="#question-D3SOX" title="Answering Questions">💬</a> <a href="https://github.com/Soundux/Soundux/pulls?q=is%3Apr+reviewed-by%3AD3SOX" title="Reviewed Pull Requests">👀</a> <a href="https://github.com/Soundux/Soundux/commits?author=D3SOX" title="Tests">⚠️</a> <a href="#translation-D3SOX" title="Translation">🌍</a> <a href="#a11y-D3SOX" title="Accessibility">️️️️♿️</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/MrKingMichael"><img src="https://avatars.githubusercontent.com/u/30067605?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Michael</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3AMrKingMichael" title="Bug reports">🐛</a> <a href="#ideas-MrKingMichael" title="Ideas, Planning, & Feedback">🤔</a> <a href="#translation-MrKingMichael" title="Translation">🌍</a> <a href="https://github.com/Soundux/Soundux/commits?author=MrKingMichael" title="Tests">⚠️</a></td>
    <td align="center"><a href="https://github.com/BrandonKMLee"><img src="https://avatars.githubusercontent.com/u/58927531?v=4?s=50" width="50px;" alt=""/><br /><sub><b>BrandonKMLee</b></sub></a><br /><a href="#ideas-BrandonKMLee" title="Ideas, Planning, & Feedback">🤔</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/Toadfield"><img src="https://avatars.githubusercontent.com/u/68649672?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Toadfield</b></sub></a><br /><a href="#ideas-Toadfield" title="Ideas, Planning, & Feedback">🤔</a> <a href="https://github.com/Soundux/Soundux/issues?q=author%3AToadfield" title="Bug reports">🐛</a></td>
    <td align="center"><a href="https://github.com/fubka"><img src="https://avatars.githubusercontent.com/u/44064746?v=4?s=50" width="50px;" alt=""/><br /><sub><b>fubka</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3Afubka" title="Bug reports">🐛</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/TheOriginalTripleD"><img src="https://avatars.githubusercontent.com/u/6907054?v=4?s=50" width="50px;" alt=""/><br /><sub><b>TheOriginalTripleD</b></sub></a><br /><a href="#research-TheOriginalTripleD" title="Research">🔬</a></td>
    <td align="center"><a href="https://github.com/UltraBlackLinux"><img src="https://avatars.githubusercontent.com/u/62404294?v=4?s=50" width="50px;" alt=""/><br /><sub><b>UltraBlackLinux</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3AUltraBlackLinux" title="Bug reports">🐛</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://bendem.be/"><img src="https://avatars.githubusercontent.com/u/2681677?v=4?s=50" width="50px;" alt=""/><br /><sub><b>bendem</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3Abendem" title="Bug reports">🐛</a></td>
    <td align="center"><a href="https://edgar.bzh/"><img src="https://avatars.githubusercontent.com/u/46636609?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Edgar Onghena</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3Aedgarogh" title="Bug reports">🐛</a> <a href="#research-edgarogh" title="Research">🔬</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/moggesmith10"><img src="https://avatars.githubusercontent.com/u/33375517?v=4?s=50" width="50px;" alt=""/><br /><sub><b>moggesmith10</b></sub></a><br /><a href="#ideas-moggesmith10" title="Ideas, Planning, & Feedback">🤔</a></td>
    <td align="center"><a href="https://belmoussaoui.com/"><img src="https://avatars.githubusercontent.com/u/7660997?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Bilal Elmoussaoui</b></sub></a><br /><a href="#platform-bilelmoussaoui" title="Packaging/porting to new platform">📦</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/thomasfinstad"><img src="https://avatars.githubusercontent.com/u/5358752?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Thomas Finstad Larsen</b></sub></a><br /><a href="#ideas-thomasfinstad" title="Ideas, Planning, & Feedback">🤔</a></td>
    <td align="center"><a href="http://arthurmelton.me"><img src="https://avatars.githubusercontent.com/u/29708070?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Arthur Melton</b></sub></a><br /><a href="#ideas-AMTitan" title="Ideas, Planning, & Feedback">🤔</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/serkan-maker"><img src="https://avatars.githubusercontent.com/u/63740626?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Serkan ÖNDER</b></sub></a><br /><a href="#translation-serkan-maker" title="Translation">🌍</a></td>
    <td align="center"><a href="https://github.com/pizzadude"><img src="https://avatars.githubusercontent.com/u/1454420?v=4?s=50" width="50px;" alt=""/><br /><sub><b>PizzaDude</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3Apizzadude" title="Bug reports">🐛</a> <a href="#research-pizzadude" title="Research">🔬</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/Kylianalex"><img src="https://avatars.githubusercontent.com/u/66625058?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Kylianalex</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3AKylianalex" title="Bug reports">🐛</a></td>
    <td align="center"><a href="http://gregerstoltnilsen.net/"><img src="https://avatars.githubusercontent.com/u/1364443?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Greger</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3Agregersn" title="Bug reports">🐛</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/rivenirvana"><img src="https://avatars.githubusercontent.com/u/43519644?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Arvin Verain</b></sub></a><br /><a href="#platform-rivenirvana" title="Packaging/porting to new platform">📦</a></td>
    <td align="center"><a href="http://einfacheinalex.eu/"><img src="https://avatars.githubusercontent.com/u/20642291?v=4?s=50" width="50px;" alt=""/><br /><sub><b>EinfachEinAlex</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/commits?author=EinfachEinAlex" title="Code">💻</a> <a href="#research-EinfachEinAlex" title="Research">🔬</a> <a href="https://github.com/Soundux/Soundux/commits?author=EinfachEinAlex" title="Tests">⚠️</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://discord.gg/ubmTQnuM3Z"><img src="https://avatars.githubusercontent.com/u/69876322?v=4?s=50" width="50px;" alt=""/><br /><sub><b>MeblIkea</b></sub></a><br /><a href="#translation-MeblIkea" title="Translation">🌍</a></td>
    <td align="center"><a href="https://nathanbonnemains.squill.fr/"><img src="https://avatars.githubusercontent.com/u/45366162?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Nathan Bonnemains</b></sub></a><br /><a href="#translation-NathanBnm" title="Translation">🌍</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/lucasvbeek"><img src="https://avatars.githubusercontent.com/u/29404838?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Lucas van Beek</b></sub></a><br /><a href="#translation-lucasvbeek" title="Translation">🌍</a></td>
    <td align="center"><a href="https://github.com/underhood"><img src="https://avatars.githubusercontent.com/u/6674623?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Timotej S.</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3Aunderhood" title="Bug reports">🐛</a> <a href="https://github.com/Soundux/Soundux/commits?author=underhood" title="Tests">⚠️</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/3JlOy-PYCCKUi"><img src="https://avatars.githubusercontent.com/u/46464602?v=4?s=50" width="50px;" alt=""/><br /><sub><b>3JlOy_PYCCKUi</b></sub></a><br /><a href="#translation-3JlOy-PYCCKUi" title="Translation">🌍</a></td>
    <td align="center"><a href="https://github.com/FuRyQC"><img src="https://avatars.githubusercontent.com/u/91005051?v=4?s=50" width="50px;" alt=""/><br /><sub><b>FuRyQC</b></sub></a><br /><a href="#translation-FuRyQC" title="Translation">🌍</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://moral.net.au"><img src="https://avatars.githubusercontent.com/u/16875700?v=4?s=50" width="50px;" alt=""/><br /><sub><b>Scott Percival</b></sub></a><br /><a href="https://github.com/Soundux/Soundux/issues?q=author%3Amoralrecordings" title="Bug reports">🐛</a> <a href="https://github.com/Soundux/Soundux/commits?author=moralrecordings" title="Tests">⚠️</a> <a href="https://github.com/Soundux/Soundux/commits?author=moralrecordings" title="Code">💻</a></td>
  </tr>
</table>

<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
