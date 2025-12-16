#pragma once
#include <Beatmap/BeatmapObjects.hpp>
#include "Scoring.hpp"

class Gauge {
public:
	Gauge() = default;
	virtual ~Gauge() = default;
	virtual bool Init(MapTotals mapTotals, uint16 total, MapTime length) = 0;
	virtual void LongHit() = 0;	
	virtual void CritHit() = 0;
	virtual void NearHit() = 0;
	virtual void LongMiss() = 0;
	virtual void ShortMiss() = 0;
	virtual void SetValue(float v) {
		m_gauge = v;
	}
	[[nodiscard]]
	virtual float GetValue() const {
		return m_gauge;
	};
	[[nodiscard]]
	virtual const std::array<float, 256>& GetSamples() const {
		return m_samples;
	}

	virtual void Update(MapTime currentTime);


	[[nodiscard]]
	virtual bool GetClearState() const = 0;
	[[nodiscard]]
	virtual const char* GetName() const = 0;
	[[nodiscard]]
	virtual GaugeType GetType() const = 0;
	[[nodiscard]]
	virtual uint32 GetOpts() const { return 0; };

	// Returns true if the gauge should fail out the player
	[[nodiscard]]
	virtual bool FailOut() const {
		return false;
	};
protected:
	virtual void InitSamples(MapTime length);

	std::array<float, 256> m_samples;
	float m_gauge = 0.0f;
	MapTime m_sampleDuration = 1;

};

class GaugeNormal : public Gauge {
public:
	GaugeNormal(float gainRate = 1.0f, float missDrainPercent = 0.02f) :
		s_gainRate(gainRate), s_missDrainPercent(missDrainPercent) {};
	~GaugeNormal() = default;
	bool Init(MapTotals mapTotals, uint16 total, MapTime length);
	void LongHit();
	void CritHit();
	void NearHit();
	void LongMiss();
	void ShortMiss();
	[[nodiscard]]
	bool GetClearState() const;
	[[nodiscard]]
	const char* GetName() const;
	[[nodiscard]]
	GaugeType GetType() const;

protected:
	const float s_gainRate = 1.0f;
	const float s_missDrainPercent = 0.02f;

	float m_shortMissDrain;
	float m_drainMultiplier;
	float m_shortGaugeGain;
	float m_tickGaugeGain;
};

class GaugeHard : public GaugeNormal {
public:
	GaugeHard(float gainRate = 12.f / 21.f, float missDrainPercent = 0.09f) :
		GaugeNormal(gainRate, missDrainPercent) {};
	~GaugeHard() = default;
	bool Init(MapTotals mapTotals, uint16 total, MapTime length) override;
	void LongMiss() override;
	void ShortMiss() override;
	[[nodiscard]]
	bool GetClearState() const;
	[[nodiscard]]
	const char* GetName() const;
	[[nodiscard]]
	bool FailOut() const;
	[[nodiscard]]
	GaugeType GetType() const;

protected:
	[[nodiscard]]
	float DrainMultiplier() const;
};

class GaugePermissive : public GaugeHard {
public:
	GaugePermissive(float gainRate = 16.f / 21.f, float missDrainPercent = 0.034f) :
		GaugeHard(gainRate, missDrainPercent) {};
protected:
	[[nodiscard]]
	const char* GetName() const;
	[[nodiscard]]
	GaugeType GetType() const;
};

class GaugeWithLevel : public GaugeHard {
public:
	GaugeWithLevel(float level, float gainRate, float missDrainPercent) :
		GaugeHard(gainRate, missDrainPercent), m_level(level) {};
	void LongMiss() override;
	void ShortMiss() override;
	[[nodiscard]]
	uint32 GetOpts() const override;
	[[nodiscard]]
	float GetLevel() const noexcept { return m_level; }
protected:
	float m_level;
};

class GaugeBlastive : public GaugeWithLevel {
public:
	GaugeBlastive(float level, float gainRate = 12.f / 21.f, float missDrainPercent = 0.04f) :
		GaugeWithLevel(level, gainRate, missDrainPercent) {};
	bool Init(MapTotals mapTotals, uint16 total, MapTime length) override;
	void NearHit() override;
	[[nodiscard]]
	const char* GetName() const;
	[[nodiscard]]
	GaugeType GetType() const;
protected:
	const float s_nearDrainPercent = 0.01f;

	float m_shortNearDrain;
};
