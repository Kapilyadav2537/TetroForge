#!/usr/bin/env python3
"""
Packaging script for TetroForge.
Compresses ONLY the files strictly necessary to build and run the game, along with README.md.
Strictly excludes tests, developer documentation, packaging scripts, and any runtime files/folders.
"""
import os
import zipfile

ALLOWED_ROOT_FILES = {"CMakeLists.txt", "CMakePresets.json", "README.md"}
ALLOWED_CMAKE_FILES = {"cmake/ProjectOptions.cmake"}

def is_necessary_file(rel_path: str) -> bool:
    norm = rel_path.replace("\\", "/")
    
    if norm in ALLOWED_ROOT_FILES or norm in ALLOWED_CMAKE_FILES:
        return True
    
    # All source and header files under src/
    if norm.startswith("src/"):
        _, ext = os.path.splitext(norm)
        if ext in {".o", ".a", ".so", ".exe", ".user", ".tmp", ".log"}:
            return False
        return True
        
    return False

def package_project(output_zip: str = "tetroforge.zip"):
    root_dir = os.path.abspath(os.path.dirname(__file__))
    file_count = 0

    print(f"Packaging TetroForge from: {root_dir}")
    print("Including only files necessary to run the game with README.md (no runtime files/folders).")

    with zipfile.ZipFile(output_zip, 'w', zipfile.ZIP_DEFLATED) as zf:
        for dirpath, _, filenames in os.walk(root_dir):
            for filename in filenames:
                full_path = os.path.join(dirpath, filename)
                rel_path = os.path.relpath(full_path, root_dir)

                if is_necessary_file(rel_path):
                    arcname = os.path.join("tetroforge", rel_path)
                    zf.write(full_path, arcname)
                    file_count += 1
                    print(f"  + {rel_path}")

    size_kb = os.path.getsize(output_zip) / 1024.0
    print(f"\nSuccessfully created {output_zip} ({size_kb:.1f} KB, {file_count} files).")

    # Check for compiled binary to also create an optional standalone binary package
    bin_candidates = [
        os.path.join(root_dir, "build", "debug", "src", "tetroforge"),
        os.path.join(root_dir, "build", "release", "src", "tetroforge"),
        os.path.join(root_dir, "build", "src", "Release", "tetroforge.exe"),
    ]
    for bin_path in bin_candidates:
        if os.path.isfile(bin_path):
            bin_zip = "tetroforge_bin.zip"
            readme_path = os.path.join(root_dir, "README.md")
            with zipfile.ZipFile(bin_zip, 'w', zipfile.ZIP_DEFLATED) as zf_bin:
                zf_bin.write(bin_path, os.path.basename(bin_path))
                if os.path.isfile(readme_path):
                    zf_bin.write(readme_path, "README.md")
            bin_size_kb = os.path.getsize(bin_zip) / 1024.0
            print(f"Also created standalone binary package: {bin_zip} ({bin_size_kb:.1f} KB) with executable + README.md.")
            break

if __name__ == "__main__":
    package_project()
