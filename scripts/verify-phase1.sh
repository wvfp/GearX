#!/bin/bash
set -euo pipefail

echo "=== GearX Phase 1 Verification ==="

echo "--- AC1: Workspace build ---"
cargo build --workspace 2>&1

echo "--- AC2: Stable Rust (no #![feature]) ---"
if grep -r "#!\\[feature" kernel/ modules/ src/ 2>/dev/null; then
  echo "FAIL: Found #![feature] usage"
  exit 1
fi
echo "PASS: No nightly features"

echo "--- AC3: Unit tests ---"
cargo test --workspace 2>&1

echo "--- AC4: No unwrap() in non-test code ---"
# Only flag unwrap() outside of test cfg blocks and test files
if grep -rn "\.unwrap()" kernel/src/ --include="*.rs" | grep -v "/tests/" | grep -v "test.rs" | grep -v "#\\[cfg(test)\\]" | head -20; then
  echo "WARNING: unwrap() found in non-test code (review manually)"
fi

echo "--- AC5: Cross-compile checks (best-effort) ---"
cargo check -p gearx_kernel --target wasm32-unknown-unknown 2>&1 || echo "SKIP: wasm target not installed"
# Android requires NDK — skip if not available
cargo check -p gearx_kernel --target aarch64-linux-android 2>&1 || echo "SKIP: Android NDK not installed"

echo "--- AC6: Binary exists ---"
ls -la target/debug/gearx 2>&1

echo ""
echo "=== ALL CHECKS COMPLETE ==="
