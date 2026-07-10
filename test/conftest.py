import http.server
import shutil
import socket
import subprocess
import threading
from pathlib import Path

import pytest


NOLIBC_DIR = Path(__file__).resolve().parents[1]


def free_port():
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        return sock.getsockname()[1]


def start_static_server(root):
    port = free_port()

    class Handler(http.server.SimpleHTTPRequestHandler):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, directory=str(root), **kwargs)

    server = http.server.HTTPServer(("127.0.0.1", port), Handler)
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()

    return server, f"http://127.0.0.1:{port}"


def compile_c_to_wasm(source, output):
    # KEEP THIS CMD IDENTICAL TO THE MAIN MAKEFILE:build_hello_wasm
    cmd = [
        "clang",
        "--target=wasm32-unknown-unknown",
        "-g",
        "-O0",
        "-static",
        "-nostdlib",
        "-nostdinc",
        "-isystem",
        str(NOLIBC_DIR / "sysroot/include"),
        "-include",
        "nolibc.h",
        "-Wl,--allow-undefined-file=wasm_undefined_symbols,--export=wasm_heap_base",
        str(source),
        "-o",
        str(output),
    ]

    subprocess.run(
        cmd,
        cwd=NOLIBC_DIR,
        check=True,
        text=True,
        capture_output=True,
    )


@pytest.fixture
def run_c_in_browser(page, tmp_path):
    def run(source_code):
        source = tmp_path / "hello.c"
        source.write_text(source_code, encoding="utf-8")

        web_root = tmp_path / "web"
        js_dir = web_root / "js_driver"
        js_dir.mkdir(parents=True)

        shutil.copy(NOLIBC_DIR / "js_driver/start.html", js_dir / "start.html")
        compile_c_to_wasm(source, web_root / "hello_wasm")

        messages = []
        page.on("console", lambda msg: messages.append(msg.text))

        server, base_url = start_static_server(web_root)
        try:
            page.goto(f"{base_url}/js_driver/start.html")
            page.wait_for_load_state("networkidle")
        finally:
            server.shutdown()

        return messages

    return run
