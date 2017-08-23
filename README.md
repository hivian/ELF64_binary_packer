# ELF64_binary_packer
ELF packer for 64-bit architecture

Basic packer:

- Create a new modified binary.
- Cypher its .text section.
- Add a new custom section which is the new entrypoint of the program.
- At runtime the new section decyphers the .text section and jump to it.

<h4>Usage:</h4>

```./packer [file_to_pack]```<br>
and<br>
```./new_binary```
