#!/usr/bin/env python3
"""
Test runner: combines solution implementations with test frameworks from practice files.
Usage: python run_tests.py <start_q> <end_q>
  e.g. python run_tests.py 1 6
"""
import subprocess, sys, os, tempfile

BASE = os.path.dirname(os.path.abspath(__file__))

with open(os.path.join(BASE, 'solutions', 'solutions.py')) as f:
    solutions_code = f.read()

def get_test_section(practice_code):
    """Extract test helpers + test runner from practice file (after TEST FRAMEWORK marker)."""
    marker = '# ============ TEST FRAMEWORK ============'
    if marker in practice_code:
        return practice_code[practice_code.index(marker):]
    # Fallback: try to find any helper functions + main block
    marker2 = 'if __name__ == "__main__":'
    if marker2 in practice_code:
        return practice_code[practice_code.index(marker2):]
    return ''

practice_dir = os.path.join(BASE, 'practice')
questions = sorted(f for f in os.listdir(practice_dir) if f.endswith('.py'))

start_q = int(sys.argv[1]) if len(sys.argv) > 1 else 1
end_q   = int(sys.argv[2]) if len(sys.argv) > 2 else 24

overall_pass = True

for fname in questions:
    try:
        q_num = int(fname[1:3])
    except ValueError:
        continue
    if not (start_q <= q_num <= end_q):
        continue

    with open(os.path.join(practice_dir, fname)) as f:
        practice_code = f.read()

    test_section = get_test_section(practice_code)
    if not test_section:
        print(f'\n=== {fname} ===')
        print('SKIP: no test section found')
        continue

    combined = solutions_code + '\n\n' + test_section

    with tempfile.NamedTemporaryFile(suffix='.py', mode='w', delete=False) as tf:
        tf.write(combined)
        tmpfile = tf.name

    try:
        result = subprocess.run(
            [sys.executable, tmpfile],
            capture_output=True, text=True, timeout=60
        )
        print(f'\n=== {fname} ===')
        if result.stdout:
            print(result.stdout.rstrip())
        if result.returncode != 0:
            overall_pass = False
            if result.stderr:
                print('STDERR:', result.stderr[:2000])
        else:
            if result.stderr:
                print('STDERR (non-fatal):', result.stderr[:500])
    except subprocess.TimeoutExpired:
        overall_pass = False
        print(f'\n=== {fname} ===')
        print('TIMEOUT after 60s')
    except Exception as e:
        overall_pass = False
        print(f'\n=== {fname} ===')
        print(f'ERROR: {e}')
    finally:
        try:
            os.unlink(tmpfile)
        except Exception:
            pass

print('\n' + '='*60)
print('Overall:', 'ALL PASSED' if overall_pass else 'SOME FAILURES')
sys.exit(0 if overall_pass else 1)
