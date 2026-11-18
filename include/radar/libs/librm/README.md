# Radar referee support

This directory contains the Radar-maintained subset of librm. The retained
code provides referee protocol revisions, frame parsing, CRC helpers, and the
device status base used by the Radar application.

The Radar build is configured from the repository root. Its only vendored
header dependencies are ETL and mapbox::eternal. The `kalman` and `rpc_core`
submodule worktrees are retained because they contain local modifications, but
they are not linked by the Radar build.
