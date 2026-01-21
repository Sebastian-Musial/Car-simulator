class SurfaceModel {
public:
    virtual ~SurfaceModel() = default;

    virtual double mu() const = 0;
};


class AsphaltSurface final : public SurfaceModel {
public:
    double mu() const override { return 0.9; }
};

class GravelSurface final : public SurfaceModel {
public:
    double mu() const override { return 0.6; }
};

class IceSurface final : public SurfaceModel {
public:
    double mu() const override { return 0.2; }
};