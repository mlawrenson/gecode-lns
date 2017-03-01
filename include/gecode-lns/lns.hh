#ifndef _LNS_H
#define _LNS_H

#include <gecode/kernel.hh>
#include <gecode/search.hh>

namespace Gecode {

    /**
     * \brief Meta-engine performing large neighborhood search
     *
     * The class \a T can implement some member functions described in the
     * \a LNSModel interface.
     *
     *
     * \ingroup TaskModelSearch
     */
    template<template<class> class E, class T>
    class LNS : public Search::Base<T> {
    public:
        /// Initialize engine for space \a s and options \a o
        LNS(T* s, const Search::Options& o);
        ~LNS(void);
        /// Return next solution (NULL, if non exists or search has been stopped)
        T* next(void);
        /// Return statistics
        Search::Statistics stats;
        Search::Statistics statistics(void) const;
        /// Check whether engine has been stopped
        bool stopped(void) const;
        static const bool best = true;
    protected:
        Space* root;
        E<T>* engine;
        E<T>* start_engine;
        const Search::Options& opt;
    };

    /**
     * \brief Perform large neighborhood search
     *
     * The engine uses the Cutoff sequence supplied in the options \a o to
     * periodically restart the search of an engine of type \a E.
     *
     * The class \a T can implement member functions
     * \code virtual void master(unsigned long int i, const Space* s) \endcode
     * and
     * \code virtual void slave(unsigned long int i, const Space* s) \endcode
     *
     * Whenever exploration restarts or a solution is found, the
     * engine executes the functions on the master and slave
     * space. For more details, consult "Modeling and Programming
     * with Gecode".
     *
     * \ingroup TaskModelSearch
     */
    template<template<class> class E, class T>
    T* lns(T* s, const Search::Options& o);

}

#include <gecode/search/support.hh>
#include <gecode/driver.hh>

namespace Gecode {
    enum LNSConstrainType { LNS_CT_NONE, LNS_CT_LOOSE, LNS_CT_STRICT, LNS_CT_SA };

    class LNSBaseOptions
    {
    public:
        virtual double initTime(void) const = 0;
        virtual void initTime(double v) = 0;

        virtual double neighborTime(void) const = 0;
        virtual void neighborTime(double v) = 0;

        virtual bool perVariable(void) const = 0;
        virtual void perVariable(bool v) = 0;

        virtual LNSConstrainType constrainType(void) const = 0;
        virtual void constrainType(LNSConstrainType v) = 0;

        virtual unsigned int maxIterationsPerIntensity(void) const = 0;
        virtual void maxIterationsPerIntensity(unsigned int v) = 0;

        virtual unsigned int minIntensity(void) const = 0;
        virtual void minIntensity(unsigned int v) = 0;

        virtual unsigned int intensityStep(void) const = 0;
        virtual void intensityStep(unsigned int v) = 0;

        virtual unsigned int maxIntensity(void) const = 0;
        virtual void maxIntensity(unsigned int v) = 0;

        virtual bool stopAtFirstNeighbor(void) const = 0;
        virtual void stopAtFirstNeighbor(bool v) = 0;

        virtual double SAstartTemperature(void) const = 0;
        virtual void SAstartTemperature(double v) = 0;

        virtual double SAcoolingRate(void) const = 0;
        virtual void SAcoolingRate(double v) = 0;

        virtual unsigned int SAneighborsAccepted(void) const = 0;
        virtual void SAneighborsAccepted(unsigned int v) = 0;
    };

    template <class OptionsBase>
    class LNSOptions : public LNSBaseOptions, public OptionsBase {
    public:
        LNSOptions(const char* p) : OptionsBase(p),
        _init_time("-lns_init_time", "LNS: the fraction of total time limit to grant for initialisation before restarting", 1.0),
        _neighbor_time("-lns_time", "LNS: the time to grant for neighborhood exploration (in milliseconds)", 10.0),
        _per_variable("-lns_per_variable", "LNS: whether the time for neighborhood exploration is intended per-variable", true),
        _stop_at_first_neighbor("-lns_stop_at_first_neighbor", "LNS: stop after finding the first neighboring solution", true),
        _constrain_type("-lns_constrain_type", "LNS: the type of constrain function to be applied to search (default: strict, other values: none, loose, sa)", LNS_CT_STRICT),
        _max_iterations_per_intensity("-lns_max_iterations_per_intensity", "LNS: max non improving iterations before increasing relaxation intensity", 10),
        _min_intensity("-lns_min_intensity", "LNS: the minimum relaxation intensity", 1),
        _intensity_step("-lns_intensity_step", "LNS: the step to increase relaxation intensity by", 1),
        _max_intensity("-lns_max_intensity", "LNS: the maximum relxation intensity", 5),
        _sa_start_temperature("-lns_sa_start_temperature", "LNS(SA): start temperature", 1.0),
        _sa_cooling_rate("-lns_sa_cooling_rate", "LNS(SA): cooling rate", 0.99),
        _sa_neighbors_accepted("-lns_sa_neighbors_accepted", "LNS(SA): neighbors accepted per temperature", 100)
        {
            _constrain_type.add(LNS_CT_NONE, "none");
            _constrain_type.add(LNS_CT_LOOSE, "loose");
            _constrain_type.add(LNS_CT_STRICT, "strict");
            _constrain_type.add(LNS_CT_SA, "sa");

            OptionsBase::add(_init_time);
            OptionsBase::add(_neighbor_time);
            OptionsBase::add(_per_variable);
            OptionsBase::add(_stop_at_first_neighbor);
            OptionsBase::add(_constrain_type);
            OptionsBase::add(_max_iterations_per_intensity);
            OptionsBase::add(_min_intensity);
            OptionsBase::add(_intensity_step);
            OptionsBase::add(_max_intensity);
            OptionsBase::add(_sa_start_temperature);
            OptionsBase::add(_sa_cooling_rate);
            OptionsBase::add(_sa_neighbors_accepted);
        }
        //    virtual void help(void);

        double neighborTime(void) const { return _neighbor_time.value(); }
        void neighborTime(double v) { _neighbor_time.value(v); }

        double initTime(void) const { return _init_time.value(); }
        void initTime(double v) { _init_time.value(v); }

        bool perVariable(void) const { return _per_variable.value(); }
        void perVariable(bool v) { _per_variable.value(v); }

        LNSConstrainType constrainType(void) const { return static_cast<LNSConstrainType>(_constrain_type.value()); }
        void constrainType(LNSConstrainType v) { _constrain_type.value(v); }

        unsigned int maxIterationsPerIntensity(void) const { return _max_iterations_per_intensity.value(); }
        void maxIterationsPerIntensity(unsigned int v) { _max_iterations_per_intensity.value(v); }

        unsigned int minIntensity(void) const { return _min_intensity.value(); }
        void minIntensity(unsigned int v) { _min_intensity.value(v); }

        unsigned int intensityStep(void) const { return _intensity_step.value(); }
        void intensityStep(unsigned int v) { _intensity_step.value(v); }

        unsigned int maxIntensity(void) const { return _max_intensity.value(); }
        void maxIntensity(unsigned int v) { _max_intensity.value(v); }

        bool stopAtFirstNeighbor(void) const { return _stop_at_first_neighbor.value(); }
        void stopAtFirstNeighbor(bool v) { _stop_at_first_neighbor.value(v); }

        double SAstartTemperature(void) const { return _sa_start_temperature.value(); }
        void SAstartTemperature(double v) { _sa_start_temperature.value(v); }

        double SAcoolingRate(void) const { return _sa_cooling_rate.value(); }
        void SAcoolingRate(double v) { _sa_cooling_rate.value(v); }

        unsigned int SAneighborsAccepted(void) const { return _sa_neighbors_accepted.value(); }
        void SAneighborsAccepted(unsigned int v) { _sa_neighbors_accepted.value(v); }

    protected:
        LNSOptions(const LNSOptions& opt)
        : OptionsBase(opt), _init_time(opt._init_time), _neighbor_time(opt._neighbor_time), _per_variable(opt._per_variable),
          _stop_at_first_neighbor(opt._stop_at_first_neighbor), _constrain_type(opt._constrain_type),
          _max_iterations_per_intensity(opt._max_iterations_per_intensity), _min_intensity(opt._min_intensity),
          _max_intensity(opt._max_intensity), _intensity_step(opt._intensity_step),
        _sa_start_temperature(opt._sa_start_temperature), _sa_cooling_rate(opt._sa_cooling_rate), _sa_neighbors_accepted(opt._sa_neighbors_accepted)
        {}
        // LNS parmeters
        Driver::DoubleOption _init_time;
        Driver::DoubleOption _neighbor_time;
        Driver::BoolOption _per_variable;
        Driver::BoolOption _stop_at_first_neighbor;
        Driver::StringOption _constrain_type;
        Driver::UnsignedIntOption _max_iterations_per_intensity;
        Driver::UnsignedIntOption _min_intensity;
        Driver::UnsignedIntOption _intensity_step;
        Driver::UnsignedIntOption _max_intensity;
        // LNS-SA specific parameters
        Driver::DoubleOption _sa_start_temperature;
        Driver::DoubleOption _sa_cooling_rate;
        Driver::UnsignedIntOption _sa_neighbors_accepted;
    };

    typedef LNSOptions<SizeOptions> LNSSizeOptions;
    typedef LNSOptions<InstanceOptions> LNSInstanceOptions;
}

namespace Gecode { namespace Search {

    /// This class implements a combined stop criterion for LNS based meta-engines
    /// the underlying engine is handled through a TimeStop, while the lns_stop is passed
    /// (possibly) from the script controlling the meta-engine.
    class LNSMetaStop : public Stop {
    protected:
        Stop* lns_stop;
        Stop* e_stop;
    public:
        LNSMetaStop(Stop* lns_stop0, TimeStop* e_stop0) : lns_stop(lns_stop0), e_stop(e_stop0) {}
        /// The stop method verifies a combined stopping condition
        /// (i.e., whether either the meta-engine or the engine stop criterion is satisfied)
        virtual bool stop(const Statistics& s, const Options& o) {
            return (e_stop != NULL && e_stop->stop(s,o)) || (lns_stop != NULL && lns_stop->stop(s,o));
        }
    };

    /// Waiting for a more integrated (and not intrusive) solution, this class is abused
    /// for passing specific parameters to the LNS engine
    class LNSParameters : public LNSInstanceOptions {
    public:
        LNSParameters(const LNSInstanceOptions& opt0) : LNSInstanceOptions(opt0) {}
    };
}}

#include "meta_lns.hh"

namespace Gecode {


    namespace Search {

        GECODE_SEARCH_EXPORT Engine* lns(Space* s, size_t sz,
                                         TimeStop* e_stop,
                                         Engine* se,
                                         Engine* e,
                                         Search::Statistics& st,
                                         const Options& o);
    }

    template<template<class> class E, class T>
    forceinline
    LNS<E,T>::LNS(T* s, const Search::Options& m_opt) : opt(m_opt) {
        Search::Options e_opt;
        e_opt.clone = true;
        e_opt.threads = m_opt.threads;
        e_opt.c_d = m_opt.c_d;
        e_opt.a_d = m_opt.a_d;
        Search::TimeStop* ts = new Search::TimeStop(0);
        Search::LNSMetaStop* ms = new Search::LNSMetaStop(m_opt.stop, ts);
        e_opt.stop = ms;
        if (m_opt.clone) {
            if (s->status(stats) == SS_FAILED) {
                stats.fail++;
                // root = new Search::FailedSpace(); // (disappeared between GECODE 4.1.0 and 4.2.0)
                root = NULL;
            } else {
                root = s->clone();
            }
        } else {
            root = s;
        }
        Search::Options& s_opt(const_cast<Search::Options&>(m_opt));
        s_opt.clone = true;
        engine = new E<T>(dynamic_cast<T*>(root),e_opt);
        Search::Engine* ee = engine->e; // FIXME: now this class has to be friend of BaseEngine to allow it
        engine->e = NULL;
        start_engine = new E<T>(dynamic_cast<T*>(root),s_opt);
        Search::Engine* se = start_engine->e;
        start_engine->e = NULL;
        this->e = Search::lns(root,sizeof(T),ts,se,ee,stats,m_opt);
    }

    template<template<class> class E, class T>
    forceinline T*
    LNS<E,T>::next(void) {
        return dynamic_cast<T*>(this->e->next());
    }

    template<template<class> class E, class T>
    forceinline Search::Statistics
    LNS<E,T>::statistics(void) const {
        return this->e->statistics();
    }

    template<template<class> class E, class T>
    forceinline bool
    LNS<E,T>::stopped(void) const {
        return this->e->stopped();
    }


    template<template<class> class E, class T>
    forceinline
    LNS<E,T>::~LNS(void) {
        if (opt.clone)
            delete root;
        //delete engine; delete start_engine;
    }


    template<template<class> class E, class T>
    forceinline T*
    lns(T* s, const Search::Options& o) {
        LNS<E,T> l(s,o);
        return l.next();
    }

}

#endif

// STATISTICS: search-other
