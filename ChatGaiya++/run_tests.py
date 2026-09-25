from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
PROGRAM = ROOT / ("chatgaiya.exe" if sys.platform == "win32" else "chatgaiya")


def run(args, **kwargs):
    return subprocess.run([str(arg) for arg in args], text=True, capture_output=True, **kwargs)


def compile_and_run(source_name, expected, stdin=None):
    with tempfile.TemporaryDirectory() as temp_dir:
        output = Path(temp_dir) / "generated.py"
        compiled = run([PROGRAM, ROOT / "tests" / source_name, "-o", output])
        assert compiled.returncode == 0, compiled.stderr
        executed = run([sys.executable, output], input=stdin)
        assert executed.returncode == 0, executed.stderr
        assert executed.stdout == expected, (source_name, executed.stdout, expected)


compile_and_run("precedence.cg", "14\n")
compile_and_run("control_flow.cg", "C\nh\na\nok\n")
compile_and_run("typed_input.cg", "42\n", "41\n")

with tempfile.TemporaryDirectory() as temp_dir:
    output = Path(temp_dir) / "should_not_exist.py"
    result = run([PROGRAM, ROOT / "tests" / "type_error.cg", "-o", output])
    assert result.returncode != 0
    assert "cannot initialize ongko with kotha" in result.stderr
    assert not output.exists()

print("All compiler smoke tests passed.")
