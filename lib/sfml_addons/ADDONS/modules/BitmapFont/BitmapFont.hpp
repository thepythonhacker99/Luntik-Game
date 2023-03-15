#ifndef BITMAP_FONT_HPP
#define BITMAP_FONT_HPP

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <cstdio>

// ASCII Printable characters range
const char FIRST_PRINTABLE_CHAR = 32;
const char LAST_PRINTABLE_CHAR = 126;

/**
 * Class for loading and manipulating bitmap fonts
 */
class BitmapFont
{
public:
	BitmapFont():
		m_width(0),
		m_height(0)
	{
	}

	/**
	 * Load a bitmap font from an image file
	 * @param imagePath: path to the bitmap font's image
	 * @param width: number of glyphs on each line
	 * @param height: number of glyphs on each column
	 * @return true if loading succeeded, false if it failed
	 */
	bool loadFromFile(const std::string& imagePath, size_t width=16, size_t height=6)
	{
		if (width > 0 && height > 0 && m_texture.loadFromFile(imagePath))
		{
			m_width = width;
			m_height = height;

			// Compute the size of a glyph from image dimensions
			m_glyphSize.x = m_texture.getSize().x / m_width;
			m_glyphSize.y = m_texture.getSize().y / m_height;
			return true;
		}
		return false;
	}

	/**
	 * Get the font's texture
	 */
	const sf::Texture& getTexture() const
	{
		return m_texture;
	}

	/**
	 * Retrieve the texture rect of a font's glyph
	 * @param character: the character to get
	 * @return texture sub rect
	 */
	sf::IntRect getGlyphRect(char character) const
	{
		if (character < FIRST_PRINTABLE_CHAR || character > LAST_PRINTABLE_CHAR)
		{
			fprintf(stderr, "BitmapFont: non printable character encoutered (ASCII %d\n)", character);
			// Replace non printable characters with '?'
			character = '?';
		}
		character -= FIRST_PRINTABLE_CHAR;

		sf::IntRect subrect;
		subrect.left = (character % m_width) * m_glyphSize.x;
		subrect.width = m_glyphSize.x;
		subrect.top = (character / m_width) * m_glyphSize.y;
		subrect.height = m_glyphSize.y;
		return subrect;
	}

	const sf::Vector2u& getGlyphSize() const
	{
		return m_glyphSize;
	}

private:
	sf::Texture  m_texture;
	size_t       m_width;
	size_t       m_height;
	sf::Vector2u m_glyphSize;
};


#endif // BITMAP_FONT_HPP
