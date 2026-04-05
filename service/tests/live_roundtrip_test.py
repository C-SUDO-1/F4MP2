#!/usr/bin/env python3
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / "build"
SERVICE = BUILD / "fo4_coop_service"
HARNESS = BUILD / "udp_client_harness"
PORT = "7799"

def main() -> int:
    if not SERVICE.exists() or not HARNESS.exists():
        print("build artifacts missing", file=sys.stderr)
        return 1
    service = subprocess.Popen([str(SERVICE), PORT], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    try:
        time.sleep(0.4)
        proc = subprocess.run([str(HARNESS), PORT, "--assert-basic"], capture_output=True, text=True, timeout=10)
        sys.stdout.write(proc.stdout)
        sys.stderr.write(proc.stderr)
        return proc.returncode
    finally:
        service.terminate()
        try:
            service.wait(timeout=2)
        except subprocess.TimeoutExpired:
            service.kill()
            service.wait(timeout=2)

if __name__ == "__main__":
    raise SystemExit(main())
