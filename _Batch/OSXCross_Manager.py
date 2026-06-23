#!/usr/bin/env python3
"""
ZenDot — macOS Build Environment Manager
Sets up and manages WSL2 + OSXCross for cross-compiling Godot macOS export templates.
"""

import tkinter as tk
from tkinter import ttk, scrolledtext, filedialog, messagebox
import subprocess
import threading
import json
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CONFIG_FILE = os.path.join(SCRIPT_DIR, "osxcross_config.json")
DEFAULT_CONFIG = {
    "osxcross_wsl_path": "~/osxcross",
    "osxcross_sdk": "",
}
DEPS = (
    "git cmake clang llvm lld python3 python3-pip scons "
    "xz-utils bzip2 cpio patch make "
    "zlib1g-dev libbz2-dev liblzma-dev libssl-dev libxml2-dev libz-dev"
)


# ── WSL helpers ────────────────────────────────────────────────────────────────
# --exec bypasses WSL's default login-shell wrapper so the binary is run directly.
# -d targets the Ubuntu distro explicitly (avoids running against the wrong default).
# /bin/sh is used because it is guaranteed present on any POSIX distro; bash may
# not be resolvable when PATH is minimal during a first-login root environment.

def _wsl(cmd, user="", timeout=300, distro=""):
    args = ["wsl"]
    if distro:
        args += ["-d", distro]
    if user:
        args += ["-u", user]
    args += ["--exec", "/bin/sh", "-c", cmd]
    return subprocess.run(args, capture_output=True, text=True,
                          encoding="utf-8", errors="replace", timeout=timeout)


def _wsl_stream(cmd, distro=""):
    args = ["wsl"]
    if distro:
        args += ["-d", distro]
    args += ["--exec", "/bin/sh", "-c", cmd]
    return subprocess.Popen(
        args,
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, encoding="utf-8", errors="replace")


def win_to_wsl(win_path):
    r = subprocess.run(
        ["wsl", "wslpath", "-u", win_path.replace("\\", "/")],
        capture_output=True, text=True, encoding="utf-8", errors="replace")
    return r.stdout.strip()


# ── App ────────────────────────────────────────────────────────────────────────

class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("ZenDot — macOS Build Environment Manager")
        self.geometry("760x660")
        self.minsize(720, 580)
        self._cfg = self._load_cfg()
        self._busy = threading.Event()
        self._distro = ""   # set once Ubuntu is detected
        self._build_ui()
        self.after(300, self.refresh_status)

    # ── Config ─────────────────────────────────────────────────────────────────

    def _load_cfg(self):
        if os.path.exists(CONFIG_FILE):
            try:
                with open(CONFIG_FILE) as f:
                    d = json.load(f)
                for k, v in DEFAULT_CONFIG.items():
                    d.setdefault(k, v)
                return d
            except Exception:
                pass
        return dict(DEFAULT_CONFIG)

    def _save_cfg(self):
        with open(CONFIG_FILE, "w") as f:
            json.dump(self._cfg, f, indent=2)

    # ── UI ──────────────────────────────────────────────────────────────────────

    def _build_ui(self):
        # Header
        hf = tk.Frame(self, bg="#16213e")
        hf.pack(fill="x")
        tk.Label(hf, text="ZenDot  ·  macOS Build Environment",
                 bg="#16213e", fg="#e8e8e8",
                 font=("Segoe UI", 12, "bold"), pady=10).pack(side="left", padx=14)

        # Status section
        sf = ttk.LabelFrame(self, text=" Environment Status ", padding=(10, 6))
        sf.pack(fill="x", padx=12, pady=(10, 3))

        self._rows = {}
        for i, (key, label) in enumerate([
            ("wsl2",     "WSL2"),
            ("ubuntu",   "Ubuntu (WSL2)"),
            ("deps",     "Build Dependencies"),
            ("osxcross", "OSXCross"),
            ("sdk",      "macOS SDK"),
        ]):
            tk.Label(sf, text=label, width=24, anchor="w",
                     font=("Segoe UI", 9)).grid(row=i, column=0, sticky="w", pady=2)
            icon = tk.Label(sf, text="…", width=2, font=("Segoe UI", 12), fg="#777")
            icon.grid(row=i, column=1, padx=(6, 10))
            info = tk.Label(sf, text="Checking…", anchor="w",
                            font=("Segoe UI", 9), width=38)
            info.grid(row=i, column=2, sticky="w")
            btn = ttk.Button(sf, width=20)
            btn.grid(row=i, column=3, padx=4, pady=1)
            btn.grid_remove()
            self._rows[key] = (icon, info, btn)

        # Config section
        cf = ttk.LabelFrame(self, text=" Configuration ", padding=(10, 6))
        cf.pack(fill="x", padx=12, pady=3)
        cf.columnconfigure(1, weight=1)

        tk.Label(cf, text="OSXCross WSL path:",
                 font=("Segoe UI", 9)).grid(row=0, column=0, sticky="w")
        self._ox_var = tk.StringVar(value=self._cfg["osxcross_wsl_path"])
        ttk.Entry(cf, textvariable=self._ox_var).grid(
            row=0, column=1, padx=6, sticky="ew", pady=2)
        ttk.Button(cf, text="Save", width=8,
                   command=self._save_ox_path).grid(row=0, column=2)

        tk.Label(cf, text="Darwin SDK override:",
                 font=("Segoe UI", 9)).grid(row=1, column=0, sticky="w", pady=(4, 0))
        sdk_row = tk.Frame(cf)
        sdk_row.grid(row=1, column=1, padx=6, sticky="w", pady=(4, 0))
        self._darwin_var = tk.StringVar(value=self._cfg.get("osxcross_sdk", ""))
        ttk.Entry(sdk_row, textvariable=self._darwin_var, width=14).pack(side="left")
        tk.Label(sdk_row, text="  leave blank to auto-detect  (e.g. darwin23)",
                 fg="#888", font=("Segoe UI", 8)).pack(side="left")
        ttk.Button(cf, text="Save", width=8,
                   command=self._save_darwin).grid(row=1, column=2, pady=(4, 0))

        tk.Label(cf, text="macOS SDK .tar.xz:",
                 font=("Segoe UI", 9)).grid(row=2, column=0, sticky="w", pady=(8, 0))
        self._sdk_file_var = tk.StringVar()
        ttk.Entry(cf, textvariable=self._sdk_file_var).grid(
            row=2, column=1, padx=6, sticky="ew", pady=(8, 0))
        ttk.Button(cf, text="Browse", width=8,
                   command=self._browse_sdk).grid(row=2, column=2, pady=(8, 0))
        tk.Label(cf,
                 text="  From a Mac:  xcode-select --install  →  "
                      "osxcross/tools/gen_sdk_package.sh",
                 fg="#888", font=("Segoe UI", 8)).grid(
            row=3, column=0, columnspan=3, sticky="w")

        # Log section
        lf = ttk.LabelFrame(self, text=" Output Log ", padding=4)
        lf.pack(fill="both", expand=True, padx=12, pady=3)
        self._log = scrolledtext.ScrolledText(
            lf, font=("Consolas", 9), bg="#0c0c0c", fg="#d4d4d4",
            insertbackground="white", state="disabled", wrap="word")
        self._log.pack(fill="both", expand=True)

        # Bottom bar
        bb = tk.Frame(self, pady=8)
        bb.pack(fill="x", padx=12)
        ttk.Button(bb, text="↺  Refresh",   width=13, command=self.refresh_status).pack(side="left")
        ttk.Button(bb, text="▶  Full Setup", width=13, command=self._full_setup).pack(side="left", padx=6)
        ttk.Button(bb, text="⬆  Install SDK", width=13, command=self._install_sdk).pack(side="left")
        ttk.Button(bb, text="Clear Log",    width=10, command=self._clear_log).pack(side="right")

    # ── Log helpers ────────────────────────────────────────────────────────────

    def _log_append(self, text):
        self._log.config(state="normal")
        self._log.insert("end", text)
        self._log.see("end")
        self._log.config(state="disabled")

    def _log_line(self, text):
        self._log_append(text + "\n")

    def _clear_log(self):
        self._log.config(state="normal")
        self._log.delete("1.0", "end")
        self._log.config(state="disabled")

    # ── Row helpers ────────────────────────────────────────────────────────────

    def _row_ok(self, key, text):
        icon, info, btn = self._rows[key]
        icon.config(text="✔", fg="#33cc66")
        info.config(text=text)
        btn.grid_remove()

    def _row_fail(self, key, text, btn_label, btn_cmd):
        icon, info, btn = self._rows[key]
        icon.config(text="✘", fg="#ff4444")
        info.config(text=text)
        btn.config(text=btn_label, command=btn_cmd)
        btn.grid()

    def _row_pending(self, key, text="Checking…"):
        icon, info, btn = self._rows[key]
        icon.config(text="…", fg="#888888")
        info.config(text=text)
        btn.grid_remove()

    # ── Config actions ──────────────────────────────────────────────────────────

    def _save_ox_path(self):
        self._cfg["osxcross_wsl_path"] = self._ox_var.get().strip()
        self._save_cfg()
        self._log_line("[Config] OSXCross path saved.")

    def _save_darwin(self):
        self._cfg["osxcross_sdk"] = self._darwin_var.get().strip()
        self._save_cfg()
        self._log_line(f"[Config] Darwin SDK override saved: '{self._cfg['osxcross_sdk']}'")

    def _browse_sdk(self):
        p = filedialog.askopenfilename(
            title="Select macOS SDK tarball",
            filetypes=[("SDK tarball", "*.tar.xz *.tar.gz"), ("All", "*")])
        if p:
            self._sdk_file_var.set(p)

    def _ox_path(self):
        return self._ox_var.get().strip() or "~/osxcross"

    # ── Status checks ─────────────────────────────────────────────────────────

    def refresh_status(self):
        for key in self._rows:
            self._row_pending(key)
        threading.Thread(target=self._run_checks, daemon=True).start()

    def _run_checks(self):
        self._check_wsl2()
        self._check_ubuntu()
        self._check_deps()
        self._check_osxcross()
        self._check_sdk()

    def _check_wsl2(self):
        r = subprocess.run(["wsl", "-l", "-v"],
                           capture_output=True, encoding="utf-16-le", errors="replace")
        ok = r.returncode == 0
        self.after(0, (lambda: self._row_ok("wsl2", "Installed"))
                   if ok else
                   (lambda: self._row_fail("wsl2", "Not installed",
                                           "How to install", self._help_wsl)))

    def _check_ubuntu(self):
        r = subprocess.run(["wsl", "-l", "-v"],
                           capture_output=True, encoding="utf-16-le", errors="replace")
        name = ""
        for line in r.stdout.splitlines():
            if "ubuntu" in line.lower():
                name = line.strip().lstrip("*").strip().split()[0]
                break
        if name:
            self._distro = name   # store for all subsequent WSL calls
            self.after(0, lambda n=name: self._row_ok("ubuntu", n))
        else:
            self.after(0, lambda: self._row_fail(
                "ubuntu", "Not found", "Install Ubuntu", self._install_ubuntu))

    def _check_deps(self):
        missing = [t for t in ("git", "cmake", "clang", "python3", "scons")
                   if _wsl(f"which {t} >/dev/null 2>&1", distro=self._distro).returncode != 0]
        if not missing:
            self.after(0, lambda: self._row_ok("deps", "All present"))
        else:
            m = ", ".join(missing)
            self.after(0, lambda: self._row_fail(
                "deps", f"Missing: {m}", "Install Deps", self._install_deps))

    def _check_osxcross(self):
        ox = self._ox_path()
        r = _wsl(f'ls "{ox}/target/bin/"*-apple-*-clang 2>/dev/null | head -1',
                 distro=self._distro)
        found = bool(r.stdout.strip())
        if found:
            # Auto-detect darwin version and write to config if not already set
            sdk_r = _wsl(
                f'ls "{ox}/target/bin/"*-apple-*-clang 2>/dev/null | head -1 '
                r'| sed "s/.*-apple-\(darwin[0-9]*\)-.*/\1/"',
                distro=self._distro)
            sdk = sdk_r.stdout.strip()
            if sdk and not self._cfg.get("osxcross_sdk"):
                self._cfg["osxcross_sdk"] = sdk
                self._save_cfg()
                self.after(0, lambda s=sdk: self._darwin_var.set(s))
            self.after(0, lambda: self._row_ok("osxcross", f"Built  ({sdk or 'sdk unknown'})"))
        else:
            self.after(0, lambda: self._row_fail(
                "osxcross", "Not built", "Build OSXCross", self._build_osxcross))

    def _check_sdk(self):
        ox = self._ox_path()
        r = _wsl(f'ls "{ox}/tarballs/MacOSX"*.tar.xz 2>/dev/null | head -1',
                 distro=self._distro)
        name = os.path.basename(r.stdout.strip()) if r.stdout.strip() else ""
        if name:
            self.after(0, lambda n=name: self._row_ok("sdk", n))
        else:
            self.after(0, lambda: self._row_fail(
                "sdk", "Not found — browse below", "Install SDK", self._install_sdk))

    # ── Install actions ────────────────────────────────────────────────────────

    def _help_wsl(self):
        messagebox.showinfo("Install WSL2",
            "1. Open PowerShell as Administrator\n"
            "2. Run:  wsl --install\n"
            "3. Restart your computer\n"
            "4. Open Ubuntu from the Start menu to complete first-run setup\n\n"
            "Then click Refresh.")

    def _install_ubuntu(self):
        subprocess.Popen([
            "powershell", "-Command",
            "Start-Process wsl -ArgumentList '--install -d Ubuntu-22.04' -Verb RunAs"])
        self._log_line("[Ubuntu] Installer launched. A restart may be required.")

    def _in_thread(self, fn, *args):
        if self._busy.is_set():
            messagebox.showinfo("Busy", "A task is already running. Please wait.")
            return
        self._busy.set()
        def wrapper():
            try:
                fn(*args)
            finally:
                self._busy.clear()
                self.after(600, self.refresh_status)
        threading.Thread(target=wrapper, daemon=True).start()

    def _install_deps(self):
        self._in_thread(self._do_install_deps)

    def _do_install_deps(self):
        self.after(0, lambda: self._log_line("[Deps] Installing via apt (may take a minute)…"))
        cmd = (f"apt-get update -qq && "
               f"DEBIAN_FRONTEND=noninteractive apt-get install -y {DEPS}")
        wsl_args = ["wsl"]
        if self._distro:
            wsl_args += ["-d", self._distro]
        wsl_args += ["-u", "root", "--exec", "/bin/sh", "-c", cmd]
        r = subprocess.run(wsl_args, capture_output=True, text=True,
                           encoding="utf-8", errors="replace")
        out = (r.stdout + r.stderr).strip()
        if out:
            self.after(0, lambda o=out: self._log_append(o + "\n"))
        self.after(0, lambda: self._log_line(
            "[Deps] ✔ Done." if r.returncode == 0
            else f"[Deps] ✘ Failed (exit {r.returncode})."))

    def _build_osxcross(self):
        ox = self._ox_path()
        r = _wsl(f'ls "{ox}/tarballs/MacOSX"*.tar.xz 2>/dev/null | head -1',
                 distro=self._distro)
        if not r.stdout.strip():
            messagebox.showwarning("SDK Required",
                "No macOS SDK tarball found in OSXCross tarballs.\n\n"
                "1. Browse for your MacOSX*.sdk.tar.xz in Configuration\n"
                "2. Click 'Install SDK'\n"
                "3. Then click Build OSXCross again.")
            return
        self._in_thread(self._do_build_osxcross, ox)

    def _do_build_osxcross(self, ox):
        self.after(0, lambda: self._log_line(f"[OSXCross] Cloning to {ox}…"))
        r = _wsl(f'[ -d "{ox}/.git" ] || '
                 f'git clone https://github.com/tpoechtrager/osxcross.git "{ox}"',
                 timeout=120, distro=self._distro)
        if r.returncode != 0:
            self.after(0, lambda: self._log_line(f"[OSXCross] ✘ Clone failed:\n{r.stderr}"))
            return
        self.after(0, lambda: self._log_line("[OSXCross] Building — 5–15 minutes…"))
        proc = _wsl_stream(f'cd "{ox}" && UNATTENDED=1 ./build.sh 2>&1',
                           distro=self._distro)
        for line in proc.stdout:
            self.after(0, lambda l=line: self._log_append(l))
        proc.wait()
        self.after(0, lambda: self._log_line(
            "\n[OSXCross] ✔ Build complete!" if proc.returncode == 0
            else f"\n[OSXCross] ✘ Build failed (exit {proc.returncode})."))

    def _install_sdk(self):
        path = self._sdk_file_var.get().strip()
        if not path or not os.path.isfile(path):
            messagebox.showwarning("No SDK selected",
                "Browse for your MacOSX*.sdk.tar.xz file first.")
            return
        self._in_thread(self._do_install_sdk, path)

    def _do_install_sdk(self, win_path):
        ox = self._ox_path()
        fname = os.path.basename(win_path)
        wsl_src = win_to_wsl(win_path)
        if not wsl_src:
            self.after(0, lambda: self._log_line(
                "[SDK] ✘ Could not convert Windows path to WSL path."))
            return
        self.after(0, lambda: self._log_line(f"[SDK] Copying {fname} → {ox}/tarballs/"))
        r = _wsl(f'mkdir -p "{ox}/tarballs" && cp "{wsl_src}" "{ox}/tarballs/{fname}"',
                 distro=self._distro)
        self.after(0, lambda: self._log_line(
            "[SDK] ✔ Copied." if r.returncode == 0
            else f"[SDK] ✘ Failed:\n{r.stderr}"))

    def _full_setup(self):
        if self._busy.is_set():
            messagebox.showinfo("Busy", "A task is already running.")
            return
        if not messagebox.askyesno("Full Setup",
            "Runs all steps in sequence:\n\n"
            "  1. Install build dependencies (apt)\n"
            "  2. Clone OSXCross from GitHub\n"
            "  3. Build OSXCross  (5–15 min)\n\n"
            "Prerequisites:\n"
            "  • WSL2 + Ubuntu must already be installed\n"
            "  • macOS SDK .tar.xz must be set in Configuration above\n\n"
            "Continue?"):
            return
        ox = self._ox_path()
        self._busy.set()
        def run():
            try:
                self._do_install_deps()
                self._do_build_osxcross(ox)
            finally:
                self._busy.clear()
                self.after(600, self.refresh_status)
        threading.Thread(target=run, daemon=True).start()


if __name__ == "__main__":
    App().mainloop()
