#include <string>

class SurfaceModel {
public:
    virtual ~SurfaceModel() = default;

    virtual double mu() const = 0;
    virtual std::string name() const= 0;
};


class AsphaltSurface final : public SurfaceModel {
public:
    double mu() const override { return 0.9; }
    std::string name() const override { return "Asphalt"; }
};

class GravelSurface final : public SurfaceModel {
public:
    double mu() const override { return 0.6; }
    std::string name() const override { return "Gravel"; }
};

class IceSurface final : public SurfaceModel {
public:
    double mu() const override { return 0.2; }
    std::string name() const override { return "Ice"; }
};