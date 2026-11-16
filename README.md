# 🔐 LSB Image Steganography in C

### Hiding Secret Data Inside BMP Images Using Least Significant Bit Manipulation

<p align="center">
  <b>A File-Hiding Implementation Using C Programming, Bitwise Operations and Binary File Handling</b>
</p>

---

## 🚀 Project Overview

What if an image could carry information that isn't immediately visible?

**LSB Image Steganography** is a C-based project that demonstrates how secret file data can be embedded inside a BMP image using the Least Significant Bit (LSB) technique.

The implementation supports both **Encoding and Decoding**, allowing users to hide a file within an image and subsequently retrieve the original content.

Rather than modifying the visible image structure significantly, the program modifies the least significant bits of image data to store the hidden information.

---

## ✨ Key Capabilities

| Feature | Description |
|---|---|
| Data Encoding | Embeds secret file data into BMP image pixels. |
| Data Decoding | Retrieves the hidden file from the stego image. |
| Capacity Validation | Checks whether the image can accommodate the secret data. |
| Magic String Verification | Uses `#*` to identify valid encoded content. |
| Metadata Preservation | Stores the secret file extension and file size. |
| BMP Header Handling | Preserves the original 54-byte BMP header. |
| File Reconstruction | Recreates the extracted file with its original extension. |

---

## ⚙️ Technical Implementation

The project demonstrates practical application of:

- **LSB Bit Manipulation:** Embedding information into the least significant bits of image data.
- **Bitwise Operators:** Using shifting, masking and bit extraction.
- **Binary File Handling:** Reading and writing image and secret files in binary mode.
- **Command-Line Arguments:** Selecting encoding or decoding operations.
- **Structures and Enumerations:** Organizing project information and operation status.
- **Modular Programming:** Separating encoding, decoding and shared definitions into individual source files.

---

## 🔄 How It Works

### 01. Encoding – Hiding the Secret File

The encoding module performs the following operations:

1. Validates the input BMP image and secret file.
2. Checks the available image capacity.
3. Copies the original BMP header.
4. Embeds the magic string and secret file metadata.
5. Encodes secret file data into the image's LSBs.
6. Copies the remaining image data.
7. Generates the resulting stego image.

### 02. Decoding – Extracting the Hidden File

The decoding module reverses the process:

1. Opens the stego image.
2. Skips the 54-byte BMP header.
3. Extracts and verifies the magic string.
4. Retrieves the secret file extension and size.
5. Extracts the hidden file data.
6. Reconstructs the original file.

---

## 🛠️ Technology Stack

| Category | Technology |
|---|---|
| Programming Language | C |
| Image Format | BMP |
| Steganography Technique | Least Significant Bit (LSB) |
| Compiler | GCC |
| Development Concepts | Pointers, Structures, Bitwise Operations, File Handling |

---

## 📁 Project Structure

```text
LSB_Image_Steganography/
│
├── main.c
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── common.h
├── types.h
├── beautiful.bmp
└── README.md
```

**Module Responsibilities**

- `main.c` – Command-line operation selection and program execution.
- `encode.c / encode.h` – Encoding implementation and function declarations.
- `decode.c / decode.h` – Decoding implementation and function declarations.
- `common.h` – Shared magic string definition.
- `types.h` – Common data types, status and operation enumerations.

---

## 💻 Build & Run

### Prerequisites
- GCC compiler
- Linux environment or WSL
- BMP source image
- Secret file to encode

### Compile

```bash
gcc main.c encode.c decode.c -o stego
```

### Encoding

```bash
./stego -e beautiful.bmp secret.txt stegno.bmp
```

### Decoding

```bash
./stego -d stegno.bmp output.txt
```

The decoding operation reconstructs the hidden file using its stored extension.

---

## 🎯 What This Project Demonstrates

This project showcases the ability to:

- Translate a data-hiding concept into a working C implementation.
- Work directly with binary image data.
- Apply bitwise operations to manipulate individual bits.
- Design separate encoding and decoding modules.
- Validate input files and available image capacity.
- Implement file extraction and reconstruction.

---

## 👩‍💻 Developer

**Vimala R P**
**GitHub Repository:** [C-LSB-Steganography](https://github.com/vimala-rp/C-LSB-Steganography)

---

<p align="center">
  <b>Turning Bitwise Operations into Practical Applications.</b>
</p>
