#include "mem/cache/prefetch/global_compression_predictor.hh"

#include "params/GlobalCompressionPredictor.hh"

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{

GlobalCompressionPredictor::GlobalCompressionPredictor(
    const GlobalCompressionPredictorParams &p) : SimObject(p),
    counter(GenericSatCounter<int32_t>(p.counter_bits))
{
    // Penalties are normalized by decompression latency.
    normalizedMissPenalty = p.miss_penalty / p.decompression_latency;
}

int32_t
GlobalCompressionPredictor::value()
{
    return counter;
}

bool
GlobalCompressionPredictor::shouldCompress()
{
    // We choose to compress data if the counter is positive. This indicates
    // that compression is helping performance.
    return counter > 0;
}

void
GlobalCompressionPredictor::update(
    GlobalCompressionPredictor::HitResult result)
{
    switch (result) {
        // Instance where compression hurts performance.
        case GlobalCompressionPredictor::HitResult::PenalizedHit:
            // Compression was used but wasn't necessary for a cache hit.
            // Reduce the counter by the (normalized) decompression latency (1)
            counter--;
            break;
        // Fall-through: Instances where compression helps performance.
        case GlobalCompressionPredictor::HitResult::AvoidedMiss:
        case GlobalCompressionPredictor::HitResult::AvoidableMiss:
            // Compression helped or would have helped. Increment the counter
            // by the (normalized) cache miss penalty.
            counter += normalizedMissPenalty;
            break;
        // Fall-through: Instances where compression has no performance effect.
        case GlobalCompressionPredictor::HitResult::UnpenalizedHit:
        case GlobalCompressionPredictor::HitResult::UnavoidableMiss:
            // Cache hit uncompressed data, or the miss could not have been
            // avoided by using compression. Do nothing to the counter.
            break;
        default:
            fatal("unreachable case");
            break;
    }
}

} // namespace prefetch
} // namespace gem5
