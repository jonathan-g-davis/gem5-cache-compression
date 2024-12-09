#ifndef __MEM_CACHE_PREFETCH_GLOBAL_COMPRESSION_PREDICTOR_HH
#define __MEM_CACHE_PREFETCH_GLOBAL_COMPRESSION_PREDICTOR_HH
#include "base/sat_counter.hh"
#include "base/types.hh"
#include "sim/sim_object.hh"
namespace gem5
{
struct GlobalCompressionPredictorParams;
GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{
/**
 * Implements the global compression predictor from "Adaptive Cache Compression
 * for High-Performance Processors" by Alameldeen et al.
 *
 * The GCP predicts whether using cache compression would benefit or degrade
 * cache performance.
 */
class GlobalCompressionPredictor : public SimObject
{
  public:
    enum HitResult
    {
      UnpenalizedHit,   // Hit against uncompressed data.
      PenalizedHit,     // Hit against unnecessarily compressed data.
      AvoidedMiss,      // Hit compressed data that would have missed.
      AvoidableMiss,    // Miss against data that would have hit if compressed.
      UnavoidableMiss,  // Miss that would have happened compressed or not.
    };
    GlobalCompressionPredictor(const GlobalCompressionPredictorParams &p);
    ~GlobalCompressionPredictor() = default;
    /**
     * Returns the value of the global saturating counter.
     *
     * Positive values means that compression is helping avoid misses, negative
     * values means that compression is hurting.
     */
    int32_t value();
    /**
     * Returns whether the cache line should be stored in a compressed format.
     *
     * This decision is made based on the sign of the global saturating
     * counter. If the value is positive, then compression is considered
     * beneficial. If the value is negative, then compression would harm cache
     * performance.
     */
    bool shouldCompress();
    /**
     * Updates the global saturating counter based on whether a cache miss was
     * avoided or was avoidable, or if the hit was penalized.
     *
     * On a penalized hit, the counter is reduced by the decompression latency.
     * Compression was harmful to performance in this case.
     *
     * On an avoided or avoidable miss, the counter is incremented by the L2
     * cache miss penalty. Compression benefited (or would have) cache
     * performance in this case.
     *
     * Unpenalized hits and unavoidable misses do not affect the counter.
     *
     * Note: The above values are normalized to the decompression latency.
     *
     * @param result The type of cache hit or miss that occurred.
     */
    void update(HitResult result);
  private:
    /**
     * The cache miss penalty, normalized by the decompression latency.
     *
     * This reduces the number of bits required in the counter.
     */
    int32_t normalizedMissPenalty;
    /**
     * Global saturating counter for measuring compression effectiveness.
     *
     * The paper uses a 19-bit signed counter that saturates at +262,143 and
     * -262,144.
     */
    GenericSatCounter<int32_t> counter;
};
} // namespace prefetch
} // namespace gem5
#endif //__MEM_CACHE_PREFETCH_GLOBAL_COMPRESSION_PREDICTOR_HH