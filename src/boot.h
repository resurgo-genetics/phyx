#ifndef SEQ_SAMP_H_
#define SEQ_SAMP_H_

class SequenceSampler {
private:
    float jkfract;
    bool jackknife;
    bool partitioned;
    vector <int> samplesites;
    vector < vector <int> > partitions;
    vector <string> partitionNames;
    vector <int> sitePartitions; // not used
    int numPartitionedSites;
    int numPartitions;
    
    vector <int> get_bootstrap_sites (int const& numchar);
    vector <int> get_jackknife_sites (int const& numchar);
    vector <int> get_partitioned_bootstrap_sites ();
    
    void parsePartitions (string & partf);
    void getPartitionParameters (vector <string> & tokens, int & start, int & stop, int & interval);
    vector <int> getPartitionSites (string const& part);
    void checkValidPartitions ();
    void calNumPartitionedSites ();
    void getSitePartitions (); // not used
    
    void findDuplicatesMissing (vector <int> const& allSites);

public:
    SequenceSampler (int const& seed, float const& jackfract, string & partf);
    vector <int> get_sampled_sites ();
    void sample_sites (int const& numchar);
    string get_resampled_seq (string const& origseq);
    int getNumPartitionedSites ();
    //~SequenceResampler();
};

#endif /* SEQ_SAMP_H_ */
