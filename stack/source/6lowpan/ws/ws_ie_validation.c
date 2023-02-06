#include "stack/source/6lowpan/ws/ws_common.h"
#include "stack/source/6lowpan/ws/ws_ie_lib.h"
#include "stack/source/6lowpan/ws/ws_llc.h"
#include "stack/mac/fhss_config.h"
#include "common/log.h"
#include "common/ws_regdb.h"

#include "ws_ie_validation.h"

static bool ws_ie_validate_chan_plan(const struct ws_generic_channel_info *rx_plan,
                                     const ws_hopping_schedule_t *hopping_schedule)
{
    const ws_channel_plan_zero_t *plan0 = &rx_plan->plan.zero;
    const ws_channel_plan_one_t *plan1 = &rx_plan->plan.one;
    const ws_channel_plan_two_t *plan2 = &rx_plan->plan.two;
    int plan_nr = rx_plan->channel_plan;
    const struct chan_params *parms = NULL;

    if (plan_nr == 1)
        return plan1->ch0 * 1000 == hopping_schedule->ch0_freq &&
               plan1->channel_spacing == hopping_schedule->channel_spacing &&
               plan1->number_of_channel == hopping_schedule->number_of_channels;
    if (plan_nr == 0)
        parms = ws_regdb_chan_params(plan0->regulatory_domain,
                                     0, plan0->operating_class);
    if (plan_nr == 2)
        parms = ws_regdb_chan_params(plan2->regulatory_domain,
                                     plan2->channel_plan_id, 0);
    if (!parms)
        return false;
    return parms->chan0_freq == hopping_schedule->ch0_freq &&
           parms->chan_count == hopping_schedule->number_of_channels &&
           ws_regdb_chan_spacing_id(parms->chan_spacing) == hopping_schedule->channel_spacing;
}

static bool ws_ie_validate_schedule(const struct ws_info *ws_info,
                                    const struct ws_generic_channel_info *chan_info,
                                    const char *ie_str)
{
    if (!ws_ie_validate_chan_plan(chan_info, &ws_info->hopping_schedule)) {
        TRACE(TR_DROP, "drop 15.4     : %s channel plan mismatch", ie_str);
        return false;
    }

    switch (chan_info->channel_function) {
    case WS_FIXED_CHANNEL:
    case WS_TR51CF:
    case WS_DH1CF:
        break;
    default:
        TRACE(TR_DROP, "drop 15.4     : %s channel function unsupported", ie_str);
        return false;
    }

    switch (chan_info->excluded_channel_ctrl) {
    case WS_EXC_CHAN_CTRL_NONE:
    case WS_EXC_CHAN_CTRL_RANGE:
    case WS_EXC_CHAN_CTRL_BITMASK:
        break;
    default:
        TRACE(TR_DROP, "drop 15.4     : %s excluded channel control unsupported", ie_str);
        return false;
    }

    return true;
}

bool ws_ie_validate_us(const struct ws_info *ws_info, const struct ws_us_ie *ie_us)
{
    return ws_ie_validate_schedule(ws_info, &ie_us->chan_plan, "US-IE");
}

bool ws_ie_validate_bs(const struct ws_info *ws_info, const struct ws_bs_ie *ie_bs)
{
    return ws_ie_validate_schedule(ws_info, &ie_bs->chan_plan, "BS-IE");
}

bool ws_ie_validate_lcp(const struct ws_info *ws_info, const struct ws_lcp_ie *ie_lcp)
{
    if (ie_lcp->chan_plan.channel_plan != 2) {
        TRACE(TR_DROP, "drop 15.4     : LCP-IE channel plan invalid");
        return false;
    }
    return ws_ie_validate_schedule(ws_info, &ie_lcp->chan_plan, "LCP-IE");
}
