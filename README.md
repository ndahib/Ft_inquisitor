# Inquisitor - ARP Poisoning Tool

A sophisticated ARP spoofing attack implementation that demonstrates man-in-the-middle (MITM) capabilities with packet sniffing and protocol analysis, specifically designed for FTP traffic interception.

## ⚠️ Legal & Ethical Disclaimer

**This tool is for educational and authorized security testing purposes ONLY.**

- Only use this on networks and systems you own or have explicit written permission to test
- Unauthorized access to computer networks is illegal under laws such as the Computer Fraud and Abuse Act (CFAA)
- The author assumes no liability for misuse or damage caused by this software
- Always follow responsible disclosure practices and local laws

---

## 📋 Table of Contents

1. [Overview](#overview)
2. [How ARP Poisoning Works](#how-arp-poisoning-works)
3. [Features](#features)
4. [Prerequisites](#prerequisites)
5. [Installation](#installation)
6. [Usage](#usage)
7. [Architecture](#architecture)
8. [Testing Environment Setup](#testing-environment-setup)
9. [File Structure](#file-structure)
10. [Implementation Details](#implementation-details)

---

## 🎯 Overview

**Inquisitor** is a command-line tool that performs bidirectional ARP spoofing attacks against targets on a local network. The tool intercepts traffic between a client and gateway, allowing real-time monitoring and analysis of network protocols—particularly FTP traffic.

### Key Capabilities

- ✅ Bidirectional (full duplex) ARP poisoning
- ✅ Automatic ARP table restoration on exit
- ✅ Real-time FTP packet sniffing and analysis
- ✅ File transfer monitoring (STOR/RETR operations)
- ✅ Graceful signal handling (SIGINT/SIGTERM)
- ✅ Input validation for IPs and MAC addresses
- ✅ Network interface flexibility

---

## 🔍 How ARP Poisoning Works

### The ARP Protocol

Address Resolution Protocol (ARP) maps IP addresses to MAC addresses on a local network. It operates at Layer 2 (Data Link) of the OSI model and is inherently **insecure**—it trusts all ARP replies without verification.

### The Attack

1. **Normal Flow**: 
   - Client ↔ Gateway communication
   
2. **Attack Flow**:
   ```
   Attacker sends:
   - ARP reply to Client:   "Gateway's IP is at MY MAC"
   - ARP reply to Gateway:  "Client's IP is at MY MAC"
   
   Result: Both machines send traffic through the attacker
   ```

3. **Traffic Interception**:
   - Packets destined for the gateway go to the attacker
   - Attacker can inspect, modify, or forward packets
   - Attacker maintains full transparency to both parties

### Why It Works

- ARP has no authentication mechanism
- Devices accept gratuitous ARP replies without verification
- Default behavior prioritizes latest information

---

## ✨ Features

### Core Functionality

| Feature | Description |
|---------|-------------|
| **Bidirectional Poisoning** | Attacks both client→gateway and gateway→client paths simultaneously |
| **ARP Restoration** | Sends corrected ARP packets to victims before exiting (5 attempts with 1s delay) |
| **FTP Monitoring** | Captures and displays FTP commands and filenames in real-time |
| **Packet Filtering** | Only processes packets on port 21 (FTP) to reduce noise |
| **Non-blocking I/O** | Uses `select()` for efficient multi-socket handling |
| **Signal Handling** | Graceful shutdown on SIGINT (Ctrl+C) and SIGTERM |
| **Input Validation** | Comprehensive IPv4 and MAC address format checking |

### Advanced Features

- Automatic gateway discovery via `/proc/net/route`
- MAC address resolution via `/proc/net/arp`
- Continuous ARP poisoning (every 2 seconds)
- Payload extraction and analysis from captured packets
- Error handling for all network operations

---

## 📦 Prerequisites

### System Requirements

- **OS**: Linux (Debian, Ubuntu, Kali, CentOS, etc.)
- **Privileges**: Root/sudo access (required for raw packet operations)
- **Architecture**: Any (x86, ARM, etc.)

### Dependencies

```bash
# Debian/Ubuntu
sudo apt-get install libpcap-dev build-essential

# Fedora/RHEL
sudo dnf install libpcap-devel gcc make

# Arch
sudo pacman -S libpcap base-devel

# macOS (if cross-compiling)
brew install libpcap
```

### Libraries Used

- **libpcap**: Packet capture and injection
- **libc**: Standard C library (sockets, networking, strings)
- **POSIX**: Signals, file operations

---

## 🚀 Installation

### Quick Start

```bash
# Clone or extract the project
cd inquisitor

# Install dependencies (automatic)
make deps

# Build the project
make

# Verify build
./inquisitor --help  # If implemented
```

### Manual Build

```bash
gcc -Wall -Wextra -Werror -I./src -fsanitize=address \
    -c src/*.c -lpcap
gcc -o inquisitor *.o -lpcap
```

### Makefile Targets

```bash
make all      # Build the executable
make clean    # Remove object files
make fclean   # Remove object files and executable
make re       # Clean rebuild
make deps     # Install dependencies
```

---

## 💻 Usage

### Basic Syntax

```bash
sudo ./inquisitor <gateway_ip> <gateway_mac> <target_ip> <target_mac>
```

### Parameters

| Parameter | Description | Format | Example |
|-----------|-------------|--------|---------|
| `gateway_ip` | IP of the default gateway/router | x.x.x.x | `192.168.1.1` |
| `gateway_mac` | MAC address of the gateway | xx:xx:xx:xx:xx:xx | `a0:b0:c0:d0:e0:f0` |
| `target_ip` | IP of the victim machine | x.x.x.x | `192.168.1.100` |
| `target_mac` | MAC address of the victim | xx:xx:xx:xx:xx:xx | `aa:bb:cc:dd:ee:ff` |

### Example Usage

```bash
# Start ARP poisoning attack on target 192.168.1.100
sudo ./inquisitor 192.168.1.1 a0:b0:c0:d0:e0:f0 192.168.1.100 aa:bb:cc:dd:ee:ff

# Output example:
# FTP Command Detected: STOR test.txt
# FTP Command Detected: RETR document.pdf
# =======================
```

### Stopping the Attack

```bash
Ctrl+C              # Sends SIGINT
# or
kill -SIGTERM <pid> # Sends SIGTERM
```

Both signals trigger:
1. ARP table restoration (5 packets to each victim)
2. Cleanup of pcap resources
3. Graceful program termination

---

## 🏗️ Architecture

### Program Flow

```
main()
  ├─ Argument validation (4 parameters required)
  ├─ Signal handler setup (SIGINT, SIGTERM)
  ├─ Input validation
  │  ├─ is_valid_ip_addr() - IPv4 format check
  │  └─ is_valid_mac_addr() - MAC format check
  ├─ Gateway discovery
  │  ├─ get_default_gateway_ip() - Reads /proc/net/route
  │  └─ get_default_gateway_mac() - Reads /proc/net/arp
  ├─ Packet construction
  │  ├─ fill_packet() - Creates ARP reply packets
  │  └─ (2 packets: one to client, one to gateway)
  └─ running_loop()
     ├─ pcap_open_live() - Open network interface
     ├─ pcap_setnonblock() - Non-blocking mode
     ├─ pcap_compile() / pcap_setfilter() - BPF filter for port 21
     ├─ select() - Event multiplexing loop
     │  ├─ FTP packet capture (pcap_dispatch)
     │  ├─ Continuous ARP poisoning (every 2 seconds)
     │  └─ Payload analysis (FTP command detection)
     └─ cleanup()
        ├─ ARP table restoration
        ├─ pcap resource cleanup
        └─ exit(1)
```

### Data Structures

#### ARP Packet Structure
```c
typedef struct s_arp_packet {
    t_eth_header eth_header;        // Ethernet frame
    u_int16_t hard_type;            // Hardware type (Ethernet)
    u_int16_t prot_type;            // Protocol type (IPv4)
    u_int8_t hard_size;             // MAC address length (6)
    u_int8_t prot_size;             // IP address length (4)
    u_int16_t operation;            // ARP reply (0x0002)
    u_int8_t sender_eth_addr[6];    // Attacker MAC
    u_int8_t sender_ip_addr[4];     // Spoofed IP
    u_int8_t target_eth_addr[6];    // Victim MAC
    u_int8_t target_ip_addr[4];     // Victim IP
} t_arp_packet;
```

### Key Functions

| Function | Purpose |
|----------|---------|
| `fill_packet()` | Constructs ARP reply packets with spoofed addresses |
| `running_loop()` | Main event loop with select() for I/O multiplexing |
| `packet_handler()` | Callback for captured packets; extracts FTP data |
| `is_valid_ip_addr()` | Validates IPv4 format (0-255 per octet) |
| `is_valid_mac_addr()` | Validates MAC format (hex, 6 octets) |
| `get_default_gateway_ip()` | Parses `/proc/net/route` to find default route |
| `get_default_gateway_mac()` | Queries `/proc/net/arp` for gateway MAC |
| `cleanup()` | Restores ARP tables and closes resources |

---

## 🧪 Testing Environment Setup

This section describes the exact testing setup used with three VMs.

### Network Topology

```
┌─────────────────────────────────────────────────────────┐
│ Local Network: 192.168.1.0/24                          │
├──────────────────┬──────────────────┬──────────────────┤
│   Gateway/Router │  Debian FTP      │   Windows 10     │
│  (192.168.1.1)   │   Server         │    (Target)      │
│                  │  (192.168.1.50)  │  (192.168.1.100) │
│                  │                  │                  │
│   MAC: a0:b0     │  MAC: bb:bb      │  MAC: cc:cc      │
│   c0:d0:e0:f0    │  cc:dd:ee:ff     │  dd:ee:ff:11     │
└──────────────────┴──────────────────┴──────────────────┘
        ▲                                       │
        │                                       │
        └───────────────────────────────────────┘
                    Kali Linux
                  (Attacker)
              192.168.1.51
          (Runs inquisitor here)
```

### VM Setup

#### 1. Kali Linux (Attacker Machine)

```bash
# Update and install tools
sudo apt-get update
sudo apt-get install -y libpcap-dev wireshark tcpdump

# Clone project
git clone <project-repo>
cd inquisitor
make all

# Verify network interface
ip addr show        # Identify interface (usually eth0, ens33, etc.)
ip route            # Verify gateway
```

#### 2. Debian (FTP Server)

```bash
# Install FTP server
sudo apt-get update
sudo apt-get install -y vsftpd

# Create test user and files
sudo useradd -m ftpuser
echo "ftpuser:password123" | sudo chpasswd
sudo mkdir -p /home/ftpuser/ftp_files
sudo touch /home/ftpuser/ftp_files/test{1,2,3}.txt
sudo chown -R ftpuser:ftpuser /home/ftpuser/ftp_files

# Configure vsftpd
sudo nano /etc/vsftpd.conf
# Set: anonymous_enable=NO
#      local_enable=YES
#      write_enable=YES

# Start FTP service
sudo systemctl restart vsftpd
sudo systemctl status vsftpd
```

#### 3. Windows 10 (Target Machine)

```powershell
# Install FTP client (if not available)
# Use built-in: ftp command
# Or install: FileZilla, WinSCP, etc.

# Enable Network Discovery for easier IP finding
Settings > Network & Internet > Advanced network settings
```

### Testing Procedure

#### Test 1: Basic ARP Poisoning (No FTP)

**Terminal 1 - Kali (Attacker)**:
```bash
# Discover MAC addresses
arp -a
# Output:
# 192.168.1.1  MAC: a0:b0:c0:d0:e0:f0 (Gateway)
# 192.168.1.100 MAC: cc:cc:dd:ee:ff:11 (Windows)

# Start poisoning
sudo ./inquisitor 192.168.1.1 a0:b0:c0:d0:e0:f0 192.168.1.100 cc:cc:dd:ee:ff:11
```

**Terminal 2 - Windows 10**:
```powershell
# Open command prompt
arp -a

# Observe: Gateway MAC changes to Kali's MAC
# 192.168.1.1 a0:b0:c0:d0:e0:f0 (original)
# Later: 192.168.1.1 <Kali MAC> (poisoned)
```

**Terminal 3 - Kali (Optional - Monitor with tcpdump)**:
```bash
sudo tcpdump -i eth0 -n "arp"
# Shows ARP replies being sent by inquisitor
```

#### Test 2: FTP Traffic Interception

**Terminal 1 - Kali (Attacker)**:
```bash
sudo ./inquisitor 192.168.1.1 a0:b0:c0:d0:e0:f0 192.168.1.100 cc:cc:dd:ee:ff:11
```

**Terminal 2 - Windows 10 (FTP Client)**:
```powershell
# Open FTP connection
ftp 192.168.1.50
# Login: ftpuser / password123

# In FTP prompt:
# Download file
get test1.txt

# Upload file
put myfile.txt

# List files
ls

# Repeat multiple times
```

**Expected Output - Kali Terminal**:
```
FTP Command Detected: RETR test1.txt
=======================

FTP Command Detected: STOR myfile.txt
=======================

FTP Command Detected: LIST
=======================
```

#### Test 3: Wireshark Monitoring (Advanced)

**Terminal 1 - Kali (Capture)**:
```bash
# In separate terminal
sudo wireshark &
# Select interface > Start capture
# Apply filter: "arp or ftp"
# Observe ARP spoofing and FTP packets
```

**Then run attack and client as above**

### Expected Test Results

| Test | Expected Behavior |
|------|-------------------|
| ARP Poisoning | Target's ARP cache updated; MAC address resolves to attacker |
| Bidirectional | Both target and gateway poisoned simultaneously |
| FTP Capture | Filenames and commands logged to console |
| Restoration | ARP tables corrected 5 times on Ctrl+C |
| No Packet Loss | Client-gateway communication continues (transparent) |

---

## 📁 File Structure

```
inquisitor/
├── Makefile                 # Build configuration
├── README.md               # This file
├── src/
│   ├── ft_arp.h           # Header file (structures, prototypes)
│   ├── main.c             # Entry point, signal handlers, main loop
│   ├── helpers.c          # IP/MAC validation, gateway discovery
│   └── packet.c           # Packet construction, FTP parsing
└── Dockerfile             # (Optional) Container image
```

### File Descriptions

#### `ft_arp.h`
- Type definitions (`t_arp_packet`, `t_eth_header`)
- Function prototypes
- Global variables
- Macro definitions for ARP operations

#### `main.c`
- `main()` - Argument parsing and initialization
- `sigint_handler()` - SIGINT/SIGTERM handling
- `cleanup()` - Resource cleanup and ARP restoration
- `running_loop()` - Main event loop with select()

#### `helpers.c`
- `is_valid_ip_addr()` - IPv4 format validation
- `is_valid_mac_addr()` - MAC format validation
- `get_default_gateway_ip()` - Gateway IP discovery
- `get_default_gateway_mac()` - Gateway MAC resolution

#### `packet.c`
- `fill_packet()` - ARP packet construction
- `packet_handler()` - Captured packet processing
- `pcap_send_packet()` - Raw packet injection
- `is_ftp_command()` - FTP command detection
- `extract_filename()` - Filename extraction from payload

---

## 🔧 Implementation Details

### ARP Poisoning Mechanism

1. **Packet Creation**:
   ```c
   // Create ARP reply claiming to be the gateway
   fill_packet(&arp_to_target, 
               gateway_ip,     // Sender IP (spoofed)
               attacker_mac,   // Sender MAC (real)
               target_ip,      // Target IP (victim)
               target_mac);    // Target MAC (victim)
   ```

2. **Continuous Poisoning**:
   ```c
   // Every 2 seconds, re-poison
   if (current_time - last_send_time >= 2) {
       pcap_sendpacket(g_handle_send, 
                      (const unsigned char*)arp_packet_to_target,
                      sizeof(t_arp_packet));
   }
   ```

3. **Bidirectional Attack**:
   - **To Target**: "Gateway IP = Attacker MAC"
   - **To Gateway**: "Target IP = Attacker MAC"
   - Both directions poisoned → Full traffic capture

### FTP Detection

```c
// Filter: port 21 (FTP)
char filter_exp[] = "port 21";
pcap_compile(g_handle_capture, &g_fp, filter_exp, ...);

// Parse FTP commands
if (strncmp((char *)payload, "STOR", 4) == 0) {
    printf("FTP Command Detected: STOR ");
    extract_filename(payload, payload_length);
}
```

### Signal Handling

```c
// Graceful shutdown
void sigint_handler(int signum) {
    g_running = false;  // Exit loop
}

// In cleanup:
// 1. Create corrected ARP packets
// 2. Send 5 times with 1-second delays
// 3. Close all file descriptors
// 4. Free allocated memory
// 5. Exit with status 1
```

### Non-blocking I/O

```c
// Use select() for efficient monitoring
select(max_fd + 1, &read_fds, NULL, NULL, &tv);

// Advantages:
// - Don't block on one operation
// - Handle multiple file descriptors
// - 1-second timeout for polling
// - CPU efficient
```

---

## 🐛 Troubleshooting

### Common Issues

| Issue | Solution |
|-------|----------|
| `Permission denied` | Run with `sudo` |
| `Couldn't open device eth0` | Check interface name with `ip addr show` |
| `Invalid IPv4 format` | Use format: `192.168.1.1` (4 octets, 0-255 each) |
| `Invalid MAC format` | Use format: `aa:bb:cc:dd:ee:ff` (lowercase hex, colons) |
| `Error setting non-blocking mode` | Check pcap version; may need update |
| `libpcap not found` | Run `make deps` to install |
| `Segmentation fault` | Verify all 4 arguments provided; check MACs are correct |

### Debug Mode

```bash
# Uncomment debug prints in source:
# print_packet_info(&arp_packet_to_target);
# dump_packet_hex(&arp_packet_to_target);

# Recompile and run to see packet details
```

### Network Monitoring

```bash
# Monitor ARP traffic in real-time
sudo tcpdump -i eth0 -n "arp"

# Monitor FTP traffic
sudo tcpdump -i eth0 -n "port 21"

# Monitor all traffic to/from target
sudo tcpdump -i eth0 -n "host 192.168.1.100"
```

---

## 📚 References & Resources

### ARP Poisoning
- [ARP Spoofing - Wikipedia](https://en.wikipedia.org/wiki/ARP_spoofing)
- [MITRE ATT&CK - Man-in-the-Middle](https://attack.mitre.org/techniques/T1557/)
- [RFC 826 - Address Resolution Protocol](https://tools.ietf.org/html/rfc826)

### Network Programming
- [libpcap Documentation](https://www.tcpdump.org/papers/sniffing-faq.html)
- [Linux Socket Programming](https://beej.us/guide/bgnet/)
- [OSI Model Layers](https://www.cisco.com/c/en/us/support/docs/security/bastion-configuration-example/5425-88.html)

### Tools Used
- [Wireshark](https://www.wireshark.org/)
- [tcpdump](https://www.tcpdump.org/)
- [arp-scan](https://github.com/royhills/arp-scan)

### Security Resources
- [OWASP Network Security](https://owasp.org/)
- [SANS Cyber Academy](https://www.sans.org/)

---

## 👤 Author

**ndahib** - Created as part of the Cybersecurity Piscine at 42 School

---

## 📝 License

This project is provided for educational purposes only. Use responsibly and legally.

---

## 🤝 Contributing

This is an educational project. Improvements welcome (fork, modify, test responsibly).

---

**Last Updated**: 2026-03-03  
**Version**: 1.0
