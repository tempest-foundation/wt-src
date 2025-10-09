// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kgeneral/kctype/kctype.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kctype.h>

/**
 * @brief Checks if a character is an uppercase letter
 *
 * @param c The character to check
 * @return Nonzero if the character is an uppercase letter, zero otherwise
 */
int
    kisupper (int c) {
	return c >= 'A' && c <= 'Z';
}

/**
 * @brief Checks if a character is a lowercase letter
 *
 * @param c The character to check
 * @return Nonzero if the character is a lowercase letter, zero otherwise
 */
int
    kislower (int c) {
	return c >= 'a' && c <= 'z';
}

/**
 * @brief Checks if a character is an alphabetic letter
 *
 * @param c The character to check
 * @return Nonzero if the character is an uppercase or lowercase letter, zero otherwise
 */
int
    kisalpha (int c) {
	return kisupper(c) || kislower(c);
}

/**
 * @brief Checks if a character is a decimal digit
 *
 * @param c The character to check
 * @return Nonzero if the character is a digit (0-9), zero otherwise
 */
int
    kisdigit (int c) {
	return c >= '0' && c <= '9';
}

/**
 * @brief Checks if a character is a hexadecimal digit
 *
 * @param c The character to check
 * @return Nonzero if the character is a hexadecimal digit (0-9, a-f, A-F), zero otherwise
 */
int
    kisxdigit (int c) {
	return kisdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

/**
 * @brief Checks if a character is alphanumeric
 *
 * @param c The character to check
 * @return Nonzero if the character is alphanumeric, zero otherwise
 */
int
    kisalnum (int c) {
	return kisalpha(c) || kisdigit(c);
}

/**
 * @brief Checks if a character is a whitespace character
 *
 * Whitespace characters include space, form feed, newline, carriage return, tab, and vertical tab.
 *
 * @param c The character to check
 * @return Nonzero if the character is a whitespace character, zero otherwise
 */
int
    kisspace (int c) {
	return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

/**
 * @brief Checks if a character is a blank character (space or tab)
 *
 * @param c The character to check
 * @return Nonzero if the character is a blank (space or tab), zero otherwise
 */
int
    kisblank (int c) {
	return c == ' ' || c == '\t';
}

/**
 * @brief Checks if a character is a control character
 *
 * Control characters are those between 0x00 and 0x1F inclusive, or 0x7F (DEL).
 *
 * @param c The character to check
 * @return Nonzero if the character is a control character, zero otherwise
 */
int
    kiscntrl (int c) {
	return (c >= 0 && c < 0x20) || c == 0x7F;
}

/**
 * @brief Checks if a character has a graphical representation
 *
 * Graphical characters are those in the range 0x21 to 0x7E inclusive.
 *
 * @param c The character to check
 * @return Nonzero if the character is graphical, zero otherwise
 */
int
    kisgraph (int c) {
	return c > 0x20 && c < 0x7F;
}

/**
 * @brief Checks if a character is printable (including space)
 *
 * Printable characters are those in the range 0x20 to 0x7E inclusive.
 *
 * @param c The character to check
 * @return Nonzero if the character is printable, zero otherwise
 */
int
    kisprint (int c) {
	return c >= 0x20 && c < 0x7F;
}

/**
 * @brief Checks if a character is a punctuation character
 *
 * Punctuation characters are printable, not alphanumeric, and not a space.
 *
 * @param c The character to check
 * @return Nonzero if the character is a punctuation character, zero otherwise
 */
int
    kispunct (int c) {
	return kisprint(c) && !kisalnum(c) && c != ' ';
}

/**
 * @brief Converts a character to lowercase if it is uppercase
 *
 * @param c The character to convert
 * @return The lowercase equivalent if the character is uppercase, otherwise returns c
 */
int
    ktolower (int c) {
	return kisupper(c) ? (c - 'A' + 'a') : c;
}

/**
 * @brief Converts a character to uppercase if it is lowercase
 *
 * @param c The character to convert
 * @return The uppercase equivalent if the character is lowercase, otherwise returns c
 */
int
    ktoupper (int c) {
	return kislower(c) ? (c - 'a' + 'A') : c;
}
