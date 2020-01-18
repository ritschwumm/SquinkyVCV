
#include "MidiPlayer4.h"
#include "MidiSong4.h"
#include "TestHost4.h"

#include "asserts.h"

static MidiSong4Ptr makeSong(int trackNum)
{
    MidiSong4Ptr song = std::make_shared<MidiSong4>();
    MidiLocker lock(song->lock);
    MidiTrackPtr clip0 = MidiTrack::makeTest(MidiTrack::TestContent::oneQ1_75, song->lock);
    MidiTrackPtr clip1 = MidiTrack::makeTest(MidiTrack::TestContent::eightQNotesCMaj, song->lock);
    assertEQ(clip0->getLength(), 4.f);
    assertEQ(clip1->getLength(), 8.f);
    
    song->addTrack(trackNum, 0, clip0);
    song->addTrack(trackNum, 1, clip1);
    return song;
}

static void testTwoSections(int trackNum)
{
    printf("---- testTwoSections \n");
    MidiSong4Ptr song = makeSong(trackNum);
    std::shared_ptr<TestHost4> host = std::make_shared<TestHost4>();
    MidiPlayer4 pl(host, song);

    // before note, nothing
    const float quantizationInterval = .01f;       
    pl.updateToMetricTime(.8, quantizationInterval, true);
   // host->assertOneActiveTrack(0);
    assertEQ(host->gateChangeCount, 0);
    assertEQ(host->gateState[0], false);

    // after note, 1
    pl.updateToMetricTime(1.1, quantizationInterval, true);
    assertEQ(host->gateChangeCount, 1);
    host->assertOneActiveTrack(trackNum);
    assertEQ(host->cvValue[0], 7.5f);
    assertEQ(host->gateState[0], true);

    // after end of note
    pl.updateToMetricTime(3.8, quantizationInterval, true);
    assertEQ(host->gateChangeCount, 2);
    host->assertOneActiveTrack(trackNum);
    assertEQ(host->cvValue[0], 7.5f);
    assertEQ(host->gateState[0], false);

    // second section, first note
    printf("about to cross sections?\n");
    pl.updateToMetricTime(4.1, quantizationInterval, true);
    host->assertOneActiveTrack(trackNum);
    assertEQ(host->gateChangeCount, 3);

    assertEQ(host->gateState[1], true);
    assertEQ(host->cvValue[1], PitchUtils::pitchToCV(3, PitchUtils::c));
    // old stuff
   // assertEQ(host->gateState[0], true);
    //assertEQ(host->cvValue[0], 1.2);
}

void testMidiPlayer4()
{
    testTwoSections(0);
}