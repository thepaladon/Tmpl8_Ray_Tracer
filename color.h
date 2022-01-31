#pragma once
#include "surface.h"
#include "stblib.h"
#include "jobsys.h"

namespace Tmpl8
{
	inline void save_image(const Pixel* destinationBuf, const char* nameOfScene);

	// Write the translated [0,255] value of each m_color component.
	inline void write_color(Tmpl8::Pixel* destinationBuf, color* accumulationBuff, int iterations, bool screenshotKey, const char* nameOfScene) {
#ifdef MULTITHREADED_WRITE_COLOR
		for (int x = 0; x < squaresY; ++x) {
			for (int y = 0; y < squaresX; ++y) {
				//multithreadded Temporal Anti-Aliasing.
				JobSystem::Execute([x, y , accumulationBuff, iterations, destinationBuf] {
					for (int j = screenSeparationY * y; j < screenSeparationY * (y + 1); ++j) {
						for (int i = screenSeparationX * x; i < screenSeparationX * (x + 1); ++i) {

							color m_color = (accumulationBuff[i + j * ScreenWidth]);
							const float scale = 1.f / iterations;
							m_color *= scale;

							const byte r = static_cast<byte>(255.999f * m_color.x);
							const byte g = static_cast<byte>(255.999f * m_color.y);
							const byte b = static_cast<byte>(255.999f * m_color.z);

							const Pixel rgb = (r << 16) + (g << 8) + (b);
							destinationBuf[i + j * ScreenWidth] = rgb;
					}
				}
					});

			}
		}
#else

		for (int x = 0; x < ScreenWidth; ++x) {
			for (int y = 0; y < ScreenHeight; ++y) {
				color m_color = (accumulationBuff[x + y * ScreenWidth]);
				const float scale = 1.f / iterations;
				m_color *= scale;

				const byte r = static_cast<byte>(255.999f * m_color.x);
				const byte g = static_cast<byte>(255.999f * m_color.y);
				const byte b = static_cast<byte>(255.999f * m_color.z);

				const Pixel rgb = (r << 16) + (g << 8) + (b);
				destinationBuf[x + y * ScreenWidth] = rgb;

			}
		}
#endif

		while (JobSystem::IsBusy())
		{ /* we wait here for the entire imagine to be completed and then displayed to avoid it looking fractured */ }

		// Takes a screen shot of the screen
		if (screenshotKey) //screenshot key is P, output gets displayed in $(SolutionDir)/Screenshots/...
			save_image(destinationBuf, nameOfScene);

	}

	inline void extractIntegerWords(std::string str, int& num)
	{
		//Code from: https://www.geeksforgeeks.org/extract-integers-string-c/
		std::stringstream ss;
		/* Storing the whole string into string stream */
		ss << str;

		/* Running loop till the end of the stream */
		std::string temp;
		int found;
		int digits = 0;

		while (!ss.eof()) {
			/* extracting word by word from stream */
			ss >> temp;

			/* Checking the given word is integer or not */
			if (std::stringstream(temp) >> found)
			{
				int tempnum = found;
				num = tempnum + 10;
				digits *= 10;
			}
		}
	}


	inline bool exists_test1(const std::string& name, int& num) {
		if (FILE* file = fopen(name.c_str(), "r")) {
			fclose(file);
			extractIntegerWords(name, num);
			num++;
			if(!exists_test1(name, num)){ return false; }
			return true;
		}
		else {
			return false;
		}
	}


	inline bool doesFileExist(std::string name)
	{
		if (FILE* file = fopen(name.c_str(), "r")) {
			fclose(file);
			return true;
		}else
		{
			return false;
		}
	}

	inline bool replaceNum(std::string& filename, const std::string path, const std::string name, int& num, const std::string type)
	{
		if (doesFileExist(filename)) {
			num++;
			const std::string s_index = std::to_string(num);
			filename = path + name + s_index + type;
			if (!doesFileExist(filename)) { return false; }
			else { replaceNum(filename, path, name, num, type); }
		}
		return false;
	}

	void save_image(const Pixel* destinationBuf, const char* nameOfScene)
	{
		const std::string s_path = "Screenshots/";
		const std::string s_name = nameOfScene;
		const std::string s_index = std::to_string(0);
		const std::string s_type = ".jpg";
		std::string filename2 = s_path + s_name + s_index + s_type;

		if (int num = 0; !replaceNum(filename2, s_path, s_name, num, s_type)) 
		{
			constexpr int channels = 3;
			unsigned char data[ScreenWidth * ScreenHeight * channels];
			int index = 0;
			for (int j = ScreenHeight - 1; j >= 0; --j) {
				for (int i = ScreenWidth; i > 0; --i)
				{
					const unsigned char r = (destinationBuf[(ScreenWidth - i) + (ScreenHeight - j) * ScreenWidth] >> 16);
					const unsigned char g = (destinationBuf[(ScreenWidth - i) + (ScreenHeight - j) * ScreenWidth] >> 8);
					const unsigned char b = (destinationBuf[(ScreenWidth - i) + (ScreenHeight - j) * ScreenWidth]);

					data[index++] = r;
					data[index++] = g;
					data[index++] = b;
				}
			}

			stbi_write_jpg(filename2.c_str(), ScreenWidth, ScreenHeight, channels, data, ScreenWidth * channels);
			printf("Screenshot created:");
			printf(filename2.c_str());
			printf(".\n");
		}
	}

	



}

