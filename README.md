# CPU-Tamer
Tame your CPU cores like a beastmaster - dynamically enable/disable cores and SMT on Linux

<div align="center">

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: Linux](https://img.shields.io/badge/platform-Linux-blue)](https://www.kernel.org/)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](CONTRIBUTING.md)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/yourname/cpu-tamer/graphs/commit-activity)

**Tame your CPU cores like a beastmaster**  
*Dynamically enable/disable cores and SMT on Linux, from "2c4t" to "4c8t" in one command.*

[Getting Started](#-getting-started) •
[Usage](#-usage) •
[Examples](#-examples) •
[How It Works](#-how-it-works) •
[FAQ](#-faq)

</div>

---

## 📖 Story Behind The Name

> My ThinkPad X1 Carbon runs Windows for 9 hours, but only 6 hours on Linux.  
> That's not acceptable. This is a matter of **Linux user dignity**.

So I built CPU-Tamer — a tool that lets me **turn off unused CPU cores** just like turning off lights in empty rooms. Now my Linux laptop **outlasts Windows**. 🏆

*CPU-Tamer: Because every core you don't use is battery you don't waste.*

---

## ✨ Features

- **🎯 Intuitive Syntax** — `cputamer set 2c4t` instead of cryptic sysfs paths
- **⚡ Dynamic Control** — Enable/disable CPU cores and SMT at runtime
- **🔋 Battery First** — Perfect for laptop users who want every last minute of battery
- **🛡️ Safety Built-in** — Won't let you disable your last core (can't lock yourself out)
- **🔍 Topology Aware** — Knows which cores share caches, optimizes your configuration
- **📊 Real-time Status** — See exactly which cores are online/offline
- **🚀 Performance Mode** — `cputamer set 4c8t` when you need full power

---

## 🚀 Getting Started

### Installation

```bash
# Clone the repository
git clone https://github.com/yourname/cpu-tamer.git
cd cpu-tamer

# Build (requires gcc and make)
make

# Install (requires root for /usr/local/bin)
sudo make install
