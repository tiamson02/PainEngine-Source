
class Light : public LightEntity
{
public:

	Vector	Color;
	float	Intensity;

    // Entity interface
    void Load(FileObject* file);
    void Save(FileObject* file);

    // serialization
    friend FileObject& operator<<(FileObject& file, Light& light)
    {
        light.Save(&file);
        return file;
    }

    friend FileObject& operator>>(FileObject& file, Light& light)
    {
        light.Load(&file);
        return file;
    }
};
