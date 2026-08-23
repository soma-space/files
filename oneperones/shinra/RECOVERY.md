# Shinra artifact recovery

## Damage in the historical bundle

Both files were introduced in commit
`d3b2bc8ad6af883b217e6b9c3270453e60657fd6` and were already damaged in that
first commit. Every known fork inherited the same blobs.

| Damaged file | Size | SHA-256 | Zero-filled from |
| --- | ---: | --- | ---: |
| `oneperone.exe` | 868756 | `e7d30134a9dcc6b8009cd1f26ff490e02f1e1bc82d46f21cf7ebc7632bfaf768` | `0x40000` |
| `ServerExtention.dll` | 415232 | `186e46194622f4e9474729f5baa619431d7325d7b63ce76c5fd2ee27f7f5eabd` | `0x40000` |

The DLL's first `0x40000` bytes are identical to Rauban's intact
`release-v0.0.48` artifact. That release contains 117637 non-zero bytes after
the boundary, demonstrating that the repository upload was truncated rather
than intentionally padded.

## Replacements in this directory

| File | Source | SHA-256 |
| --- | --- | --- |
| `oneperone.exe` | Rebuilt extension loader | `b987276c8b50b50b5610c080a6943f75d081a86ee5c9484a9783ca6e7975beef` |
| `ServerExtention.dll` | Rauban `release-v0.0.62` | `db47d3eab4ce663f044ef8fb4941ca02d68cffbc831185fb094090784c30dafa` |
| `gsodbc.dll` | Rauban source commit `36e4c47d9067c7ed0cfe3f2d7b2a209051585eb8` | `7645b10803c17ef078a8337314337c4a3f2453383c67db8046a76b9ffd51a0e9` |
| `SQL.ini` | Extension connection configuration | `11d6fc9e8d09d6803ac2d1f7b8e139c53416b8279e72b3a7be48d1a0c6f49dac` |

The replacement executable begins with Arcanine's intact `OnePerOne.exe` from
files commit `820d9c865f6e53d3ab50b9a191491f84d6e30f66` (SHA-256
`c4cc772cf5e97b477e2b293a1da67fec4e547504aeef7b43e857da723a48fb55`).
The auditable patcher and full reconstruction notes live in
[`soma-space/myth-of-soma-server-windows`](https://github.com/soma-space/myth-of-soma-server-windows/blob/main/docs/RECOVERY.md).

The new loader applies the two surviving Shinra code changes and adds a small
`.somaext` section that loads `ServerExtention.dll` before the original entry
point. It is not a byte-for-byte reconstruction of the missing LordPE section.
It has been tested with the real extension, 32-bit Microsoft ODBC driver, and a
restored Soma database.
