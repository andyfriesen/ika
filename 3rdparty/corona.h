    /**
 * Corona Image I/O Library
 * (c) 2002 Chad Austin
 *
 * This API uses principles explained at
 * http://aegisknight.org/cppinterface.html
 *
 * This code licensed under the terms of the zlib license.  See
 * documentation/license.txt.
 */


#ifndef CORONA_H
#define CORONA_H


#ifndef __cplusplus
#error Corona requires C++
#endif
  

/* calling convention */
#ifdef _WIN32
#  define COR_CALL __stdcall
#else
#  define COR_CALL
#endif


#define COR_FUNCTION(ret, decl) extern "C" ret COR_CALL decl


namespace corona {

  /**
   * File formats supported for reading or writing.
   */
  enum FileFormat {
    FF_AUTODETECT = 0x0100,
    FF_PNG        = 0x0101,
    FF_JPEG       = 0x0102,
    FF_PCX        = 0x0103,
    FF_BMP        = 0x0104,
    FF_TGA        = 0x0105,
    FF_GIF        = 0x0106,
  };

  /**
   * Pixel format specifications.  Pixel data can be packed in one of
   * the following ways.
   */
  enum PixelFormat {
    PF_DONTCARE = 0x0200,  /**< special format used when specifying a
                                desired pixel format */
    PF_R8G8B8A8 = 0x0201,  /**< RGBA, channels have eight bits of precision */
    PF_R8G8B8   = 0x0202,  /**< RGB, channels have eight bits of precision  */
    PF_I8       = 0x0203,  /**< Palettized, 8-bit indices into palette      */
  };


  /**
   * An image object represents a rectangular collections of pixels.
   * They have a width, a height, and a pixel format.  Images cannot
   * be resized.
   */
  class Image {
  public:

    /**
     * Destroy the image object, freeing the pixel buffer and any
     * associated memory.
     */
    virtual void destroy() = 0;

    /**
     * Get image width.
     * @return  image width
     */
    virtual int getWidth() = 0;

    /**
     * Get image height.
     * @return  image height
     */
    virtual int getHeight() = 0;

    /**
     * Get pixel format.
     * @return  pixel format
     */
    virtual PixelFormat getFormat() = 0;

    /**
     * Get pixel buffer.  The pixels are packed in the format defined
     * by the image's pixel format.
     *
     * @return  pointer to first element in pixel buffer
     */
    virtual void* getPixels() = 0;

    /**
     * Get the palette.  Pixels are packed in the format defined by
     * getPaletteFormat().
     *
     * @return  pointer to first palette entry
     */
    virtual void* getPalette() = 0;

    /**
     * Get the number of entries in the palette.
     *
     * @return  number of palette entries
     */
    virtual int getPaletteSize() = 0;

    /**
     * Get the format of the colors in the palette.
     *
     * @return  pixel format of palette entries
     */
    virtual PixelFormat getPaletteFormat() = 0;

    /**
     * "delete image" should actually call image->destroy(), thus putting the
     * burden of calling the destructor and freeing the memory on the image
     * object, and thus on Corona's side of the DLL boundary.
     */
    void operator delete(void* p) {
      if (p) {
        Image* i = static_cast<Image*>(p);
        i->destroy();
      }
    }
  };


  /**
   * Represents a random-access file, usually stored on a disk.  Files
   * are always binary: that is, they do no end-of-line
   * transformations.  File objects are roughly analogous to ANSI C
   * FILE* objects.
   */
  class File {
  protected:
    /**
     * Protected destructor.  Use close().
     */
    ~File() { }

  public:

    /**
     * The different ways you can seek within a file.
     */
    enum SeekMode {
      BEGIN,    /**< relative to the beginning of the file */
      CURRENT,  /**< relative to the current position in the file */
      END       /**< relative to the end of the file: position should
                     be negative*/
    };

    /**
     * Close the file and destroy the file object.
     */
    virtual void close() = 0;

    /**
     * Read size bytes from the file, storing them in buffer.
     *
     * @param buffer  
     * @param size    
     *
     * @return  number of bytes successfully read
     */
    virtual int read(void* buffer, int size) = 0;

    /**
     * Write size bytes from buffer to the file.
     *
     * @param buffer  buffer that contains the data to write
     * @param size    number of bytes to write
     *
     * @return  number of bytes successfully written
     */
    virtual int write(void* buffer, int size) = 0;

    /**
     * Jump to a new position in the file, using the specified seek
     * mode.  Remember: if mode is END, the position must be negative,
     * to seek backwards from the end of the file into its contents.
     * If the seek fails, the current position is undefined.
     *
     * @param position  position relative to the mode
     * @param mode      where to seek from in the file
     *
     * @return  true on success, false otherwise
     */
    virtual bool seek(int position, SeekMode mode) = 0;

    /**
     * Get current position within the file.
     *
     * @return  current position
     */
    virtual int tell() = 0;
  };


  /**
   * FileSystem objects represent a heirarchical collection of files.
   * In this particular case, they can even be simplified down to a
   * mapping from names to file objects.
   *
   * Files can be opened in read-only, write-only, or read-write
   * modes.  All files are treated as binary files.  That is, no
   * processing is done to end-of-line markers.
   */
  class FileSystem {
  protected:
    /**
     * You can't manually delete files.  Use destroy() instead.
     */
    ~FileSystem() { }

  public:

    /**
     * openFile() mode bitmasks.  OR these together to combine them.
     */
    enum OpenMode {
      READ   = 0x0001,  /**< open file in read-only mode */
      WRITE  = 0x0002,  /**< open file in write-only mode */
    };

    /**
     * Destroy the filesystem object.
     */
    virtual void destroy() = 0;

    /**
     * Open a file from the filesystem.
     *
     * @param filename  name of the file in the filesystem
     * @param mode      file mode
     *
     * @return  new file object on success, 0 on failure
     */
    virtual File* openFile(const char* filename, OpenMode mode) = 0;
  };


  /** PRIVATE API - for internal use only */
  namespace hidden {

    // these are extern "C" so we don't mangle the names


    // API information

    COR_FUNCTION(const char*, CorGetVersion());

    // creation

    COR_FUNCTION(Image*, CorCreateImage(
      int width,
      int height,
      PixelFormat format));

    COR_FUNCTION(Image*, CorCreatePalettizedImage(
      int width,
      int height,
      PixelFormat format, // must be a palettized format
      int palette_size,
      PixelFormat palette_format));

    COR_FUNCTION(Image*, CorCloneImage(
      Image* source,
      PixelFormat format));

    // loading

    COR_FUNCTION(Image*, CorOpenImage(
      const char* filename,
      FileFormat file_format));

    COR_FUNCTION(Image*, CorOpenImageFromFileSystem(
      FileSystem* fs,
      const char* filename,
      FileFormat file_format));

    COR_FUNCTION(Image*, CorOpenImageFromFile(
      File* file,
      FileFormat file_format));

    // saving

    COR_FUNCTION(bool, CorSaveImage(
      const char* filename,
      FileFormat file_format,
      Image* image));

    COR_FUNCTION(bool, CorSaveImageToFileSystem(
      FileSystem* fs,
      const char* filename,
      FileFormat file_format,
      Image* image));

    COR_FUNCTION(bool, CorSaveImageToFile(
      File* file,
      FileFormat file_format,
      Image* image));

    // conversion

    COR_FUNCTION(Image*, CorConvertImage(
      Image* image,
      PixelFormat format));
  }


  /* PUBLIC API */


  /**
   * Return the Corona version string.
   *
   * @return  Corona version information
   */
  inline const char* GetVersion() {
    return hidden::CorGetVersion();
  }

  /**
   * Create a new, blank image with a specified width, height, and
   * format.
   *
   * @param width   width of the new image
   * @param height  height of the new image
   * @param format  format the pixels are stored in, cannot be PF_DONTCARE
   *
   * @return  newly created blank image
   */
  inline Image* CreateImage(
    int width,
    int height,
    PixelFormat format)
  {
    return hidden::CorCreateImage(width, height, format);
  }

  /**
   * Create a new, blank image with a specified width, height, format,
   * and palette.
   *
   * @param width           width of image
   * @param height          height of image
   * @param format          format of palette indices, should be PF_I8
   * @param palette_size    number of colors in palette
   * @param palette_format  pixel format of palette entries
   */
  inline Image* CreateImage(
    int width,
    int height,
    PixelFormat format,
    int palette_size,
    PixelFormat palette_format)
  {
    return hidden::CorCreatePalettizedImage(
      width, height, format,
      palette_size, palette_format);
  }

  /**
   * Create a new image from an old one.  If format is specified, the
   * new image is converted to that pixel format.  If format is not
   * specified, the new image simply uses the same format as the
   * source.  If the image could not be cloned or the pixel format is
   * invalid, CloneImage returns 0.
   *
   * @param source  image to clone
   * @param format  format the new image is stored in, defaults to PF_DONTCARE
   *
   * @return  new image cloned from the source, 0 if failure
   */
  inline Image* CloneImage(
    Image* source,
    PixelFormat format = PF_DONTCARE)
  {
    return hidden::CorCloneImage(source, format);
  }

  /**
   * Opens an image from the default filesystem.  This function simply
   * forwards the call to OpenImage(fs, filename, file_format,
   * pixel_format) with the default filesystem object.
   *
   * See OpenImage(fs, filename, file_format, pixel_format) for more
   * information.
   *
   * @param filename      image filename to open
   * @param file_format   file format the image is stored in, or FF_AUTODETECT
   *                      to try all loaders
   * @param pixel_format  desired pixel format, or PF_DONTCARE to use image's
   *                      native format
   *
   * @return  the image loaded from the disk, or 0 if it cannot be opened
   */
  inline Image* OpenImage(
    const char* filename,
    FileFormat file_format = FF_AUTODETECT,
    PixelFormat pixel_format = PF_DONTCARE)
  {
    return hidden::CorConvertImage(
      hidden::CorOpenImage(filename, file_format),
      pixel_format);
  }

  /**
   * Opens an image from the specified filesystem.  This function
   * simply opens a file from the filesystem and passes it to
   * OpenImage(file, file_format, pixel_format).
   *
   * See OpenImage(file, file_format, pixel_format) for more
   * information.
   *
   * @param fs            filesystem to load the image from
   * @param filename      name of the file that contains the image
   * @param file_format   file format the image is stored in, or FF_AUTODETECT
   *                      to try all loaders
   * @param pixel_format  desired pixel format, or PF_DONTCARE to use image's
   *                      native format
   *
   * @return  the image loaded from the file, or 0 if it cannot be opened
   */
  inline Image* OpenImage(
    FileSystem* fs,
    const char* filename,
    FileFormat file_format = FF_AUTODETECT,
    PixelFormat pixel_format = PF_DONTCARE)
  {
    return hidden::CorConvertImage(
      hidden::CorOpenImageFromFileSystem(fs, filename, file_format),
      pixel_format);
  }

  /**
   * Opens an image from the specified file.
   *
   * If file_format is FF_AUTODETECT, the loader tries
   * to load each format until it finds one that succeeds.  Otherwise,
   * it tries the specific loader specified.
   *
   * If pixel_format is PF_DONTCARE, the new image object has the
   * pixel format closest to the image's format on disk.  Otherwise,
   * the pixels are converted to the specified format before the image
   * is returned.
   *
   * @param file          name of the file that contains the image
   * @param file_format   file format the image is stored in, or FF_AUTODETECT
   *                      to try all loaders
   * @param pixel_format  desired pixel format, or PF_DONTCARE to use image's
   *                      native format
   *
   * @return  the image loaded from the file, or 0 if it cannot be opened
   */
  inline Image* OpenImage(
    File* file,
    FileFormat file_format = FF_AUTODETECT,
    PixelFormat pixel_format = PF_DONTCARE)
  {
    return hidden::CorConvertImage(
      hidden::CorOpenImageFromFile(file, file_format),
      pixel_format);
  }

  /**
   * Saves an image to a file in the default filesystem.  This
   * function simply calls SaveImage(fs, filename, file_format, image)
   * with the default filesystem.
   *
   * See SaveImage(fs, filename, file_format, image) for more information.
   *
   * @param filename     name of the file to save the image to
   * @param file_format  file format in which to save image -- must not be
   *                     FF_AUTODETECT
   * @param image        image to save
   *
   * @return  true if save succeeds, false otherwise
   */
  inline bool SaveImage(
    const char* filename,
    FileFormat file_format,
    Image* image)
  {
    return hidden::CorSaveImage(filename, file_format, image);
  }

  /**
   * Saves an image to a file in the specified filesystem.  This
   * function simply opens a file from the filesystem and calls
   * SaveImage(file, file_format, image).
   *
   * See SaveImage(file, file_format, image) for more information.
   *
   * @param fs           filesystem used to open output file
   * @param filename     name of output file in which image is saved
   * @param file_format  file format in which to save image -- must not be
   *                     FF_AUTODETECT
   * @param image        image to save
   *
   * @return  true if save succeeds, false otherwise
   */
  inline bool SaveImage(
    FileSystem* fs,
    const char* filename,
    FileFormat file_format,
    Image* image)
  {
    return hidden::CorSaveImageToFileSystem(fs, filename, file_format, image);
  }

  /**
   * Saves an image to the specified file.  This function saves image
   * to a file of type file_format.  If file_format is not a supported
   * output type, the function fails.  As of now, Corona only supports
   * saving images of type FF_PNG.
   *
   * @param file         file in which to save the image
   * @param file_format  file format in which to save image -- must not be
   *                     FF_AUTODETECT
   * @param image        image to save
   *
   * @return  true if the save succeeds, false otherwise
   */
  inline bool SaveImage(
    File* file,
    FileFormat file_format,
    Image* image)
  {
    return hidden::CorSaveImageToFile(file, file_format, image);
  }

  /**
   * Converts an image from one format to another, destroying the old
   * image.  If source is 0, the function returns 0.  If format is
   * PF_DONTCARE or the source and target formats match, returns the
   * unmodified source image.  If a valid conversion is not found,
   * ConvertImage destroys the old image and returns 0.  For example,
   * ConvertImage does not support creating a palettized image from a
   * direct color image yet.
   *
   * @param source  image to convert
   * @param format  desired format -- can be PF_DONTCARE
   *
   * @return  valid image object if conversion succeeds, 0 otherwise
   */
  inline Image* ConvertImage(Image* source, PixelFormat format) {
    return hidden::CorConvertImage(source, format);
  }

}


#endif
