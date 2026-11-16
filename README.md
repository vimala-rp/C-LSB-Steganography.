# LSB Image Steganography

### Hide Secret Data Inside BMP Images Using Least Significant Bit (LSB) Technique

## 📌 Project Overview

LSB Image Steganography is a C programming project that allows users to hide secret file data inside a BMP image using the Least Significant Bit (LSB) technique.

The project implements both encoding and decoding operations, allowing users to embed confidential data into an image and retrieve the original file whenever required.

## 🎯 Project Objectives

- Understand the Least Significant Bit (LSB) technique.
- Implement data hiding using C programming.
- Perform file handling using binary read and write operations.
- Implement encoding and decoding mechanisms.
- Retrieve hidden data with its original file extension.

## ⚙️ Technologies Used

- **Programming Language:** C
- **Image Format:** BMP
- **Technique:** Least Significant Bit (LSB)
- **Concepts:** Bitwise Operations, Pointers, File Handling, Structures, Functions

## ✨ Key Features

- Hides secret file data inside BMP images.
- Checks image capacity before encoding.
- Uses a magic string (`#*`) to identify encoded data.
- Stores file extension and file size as metadata.
- Extracts hidden data during decoding.
- Preserves the original BMP header and remaining image data.

## 🔄 Project Workflow

### 1. Encoding Process

1. Validate command-line arguments and input files.
2. Open the source BMP image and secret file.
3. Check whether the image has sufficient capacity.
4. Copy the 54-byte BMP header.
5. Embed the magic string and secret file metadata.
6. Encode secret file data into image LSBs.
7. Copy the remaining image data.
8. Generate the stego image.

### 2. Decoding Process

1. Open the stego image.
2. Skip the 54-byte BMP header.
3. Decode and verify the magic string.
4. Retrieve the secret file extension and size.
5. Extract the hidden file data.
6. Reconstruct the original file.

## 📂 Project Structure

```text
LSB_Image_Steganography/
│
├── common.h
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── main.c
├── types.h
├── beautiful.bmp
└── README.md
```

## 💻 Compilation

Compile the project using GCC:

```bash
gcc main.c encode.c decode.c -o stego
```

## ▶️ Execution

### Encoding

```bash
./stego -e beautiful.bmp secret.txt stegno.bmp
```

### Decoding

```bash
./stego -d stegno.bmp output.txt
```

**Note:** The decoding operation reconstructs the hidden file using its stored extension.

## 🧠 Concepts Learned

- Least Significant Bit manipulation
- Bitwise operators
- Binary file handling
- Command-line argument handling
- Structures and enumerations
- Modular programming in C
- File size and image capacity calculations

## 🚀 Conclusion

This project demonstrates the practical implementation of image steganography using C programming. It provides hands-on experience with bitwise operations, binary file handling, and modular program design through encoding and decoding operations.