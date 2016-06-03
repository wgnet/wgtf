#include "test_list_model.hpp"
#include "test_list_item.hpp"

#include "core_data_model/i_item_role.hpp"

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <array>
#include <string>
#include <memory>
#include <thread>
#include <random>

namespace wgt
{
struct StringList2
{
	StringList2()
	{
		position = 0;
		listOfData = "aardvark aardwolf aaron aback abacus abaft abalone abandon abandoned abandonment abandons abase abased abasement abash abashed abate abated abatement abates abattoir abattoirs abbe abbess abbey abbeys abbot abbots abbreviate abbreviated abbreviates abbreviating abbreviation abbreviations abdicate abdicated abdicates abdicating abdication abdomen abdomens abdominal abduct abducted abducting abduction abductions abductor abductors abducts abe abeam abel abele aberdeen aberrant aberration aberrations abet abets abetted abetting abeyance abhor abhorred abhorrence abhorrent abhors abide abided abides abiding abidjan abies abilities ability abject abjectly abjure abjured ablate ablates ablating ablation ablative ablaze able ablebodied abler ablest abloom ablution ablutions ably abnegation abnormal abnormalities abnormality abnormally aboard abode abodes abolish abolished abolishes abolishing abolition abolitionist abolitionists abomb abominable abominably abominate abominated abomination abominations aboriginal aborigines abort aborted aborting abortion abortionist abortionists abortions abortive aborts abound abounded abounding abounds about above abraded abraham abrasion abrasions abrasive abrasively abrasiveness abrasives abreast abridge abridged abridgement abridging abroad abrogate abrogated abrogating abrogation abrogations abrupt abruptly abruptness abscess abscesses abscissa abscissae abscissas abscond absconded absconder absconding absconds abseil abseiled abseiler abseiling abseils absence absences absent absented absentee absenteeism absentees absenting absently absentminded absentmindedly absentmindedness absolute absolutely absoluteness absolutes absolution absolutism absolutist absolutists absolve absolved absolves absolving absorb absorbed absorbency absorbent absorber absorbers absorbing absorbingly absorbs absorption absorptions absorptive absorptivity abstain abstained abstainer abstainers abstaining abstains abstemious abstemiously abstemiousness abstention abstentions abstinence abstinent abstract abstracted abstractedly abstracting abstraction abstractions abstractly abstracts abstruse abstrusely absurd absurder absurdest absurdist absurdities absurdity absurdly abundance abundances abundant abundantly abuse abused abuser abusers abuses abusing abusive abusively abusiveness abut abutment abutments abutted abutting abuzz aby abysmal abysmally abyss abyssal abysses acacia academe academia academic academical academically academician academicians academics academies academy acanthus acapulco accede acceded acceding accelerate accelerated accelerates accelerating acceleration accelerations accelerator accelerators accelerometer accelerometers accent accented accenting accents accentuate accentuated accentuates accentuating accentuation accept acceptability acceptable acceptably acceptance acceptances accepted accepting acceptor acceptors accepts access accessed accesses accessibility accessible accessing accession accessions accessories accessory accidence accident accidental accidentally accidentprone accidents acclaim acclaimed acclaims acclamation acclamations acclimatisation acclimatise acclimatised acclimatising accolade accolades accommodate accommodated accommodates accommodating accommodation accommodations accompanied accompanies accompaniment accompaniments accompanist accompany accompanying accomplice accomplices accomplish accomplished accomplishes accomplishing accomplishment accomplishments accord accordance accorded according accordingly accordion accordionist accordions accords accost accosted accosting accosts account accountability accountable accountancy accountant accountants accounted accounting accounts accra accredit accreditation accredited accrediting accredits accreted accretion accretions accrual accruals accrue accrued accrues accruing accumulate accumulated accumulates accumulating accumulation accumulations accumulative accumulator accumulators accuracies accuracy accurate accurately accursed accusal accusals accusation accusations accusative accusatory accuse accused accuser accusers accuses accusing accusingly accustom accustomed accustoming ace aced acentric acerbic acerbity acers aces acetal acetate acetates acetic acetone acetylene ache ached aches achievable achieve achieved achievement achievements achiever achievers achieves achieving aching achingly achings achromatic achy acid acidic acidification acidified acidify acidifying acidity acidly acidophiles acidrain acids acknowledge acknowledged acknowledgement acknowledgements acknowledges acknowledging acknowledgment acknowledgments acme acne acolyte acolytes aconite acorn acorns acoustic acoustical acoustically acoustics acquaint acquaintance acquaintances acquainted acquainting acquaints acquiesce acquiesced acquiescence acquiescent acquiescing acquire acquired acquirer acquirers acquires acquiring acquisition acquisitions acquisitive acquisitiveness acquit acquited acquites acquits acquittal acquittals ";
		listOfData += "acquittance acquitted acquitting acre acreage acres acrid acrimonious acrimoniously acrimony acrobat acrobatic acrobatics acrobats acronym acronyms across acrostic acrostics acrylic acrylics act acted acting actings actinides action actionable actions activate activated activates activating activation activations activator activators active actively actives activism activist activists activities activity actor actors actress actresses acts actual actualisation actualise actualised actualities actuality actually actuarial actuaries actuary actuate actuated actuates actuating actuation actuator actuators acuity acumen acupuncture acupuncturist acupuncturists acute acutely acuteness acuter acutest acyclic adage adages adagio adam adamant adamantly adapt adaptability adaptable adaptation adaptations adapted adapter adapters adapting adaptive adaptively adaptivity adaptor adaptors adapts add added addenda addendum adder adders addict addicted addiction addictions addictive addictiveness addicts adding addition additional additionally additions additive additively additives addle addled addles addling address addressability addressable addressed addressee addressees addresses addressing adds adduce adduced adduces adducing adelaide aden adenine adenoid adenoids adenoma adenomas adept adepts adequacy adequate adequately adhere adhered adherence adherent adherents adherer adherers adheres adhering adhesion adhesions adhesive adhesiveness adhesives adhoc adiabatic adiabatically adieu adieus adieux adios adipose adit adjacency adjacent adjacently adjectival adjective adjectives adjoin adjoined adjoining adjoins adjourn adjourned adjourning adjournment adjourns adjudge adjudged adjudges adjudicate adjudicated adjudicates adjudicating adjudication adjudications adjudicator adjudicators adjunct adjuncts adjure adjust adjustable adjusted adjuster adjusting adjustment adjustments adjusts adjutant adlib adlibs adman admen admin administer administered administering administers administrate administrated administrating administration administrations administrative administratively administrator administrators admirable admirably admiral admirals admiration admire admired admirer admirers admires admiring admiringly admissibility admissible admission admissions admit admits admittance admittances admitted admittedly admitting admix admixture admonish admonished admonishes admonishing admonishment admonition admonitions admonitory ado adobe adolescence adolescent adolescents adonis adopt adopted adopter adopting adoption adoptions adoptive adopts adorable adorably adoration adore adored adorer adorers adores adoring adoringly adorn adorned adorning adornment adornments adorns adrenal adrenalin adrenaline adrift adroit adroitly adroitness adsorb adsorbed adsorption adulation adulatory adult adulterate adulterated adulterates adulterating adulteration adulterations adulterer adulterers adulteress adulteresses adulterous adultery adulthood adults adumbrate adumbrated adumbrating advance advanced advancement advancements advancer advances advancing advantage advantaged advantageous advantageously advantages advent advents adventure adventured adventurer adventurers adventures adventuring adventurism adventurous adventurously adverb adverbial adverbs adversarial adversaries adversary adverse adversely adversities adversity advert adverted advertise advertised advertisement advertisements advertiser advertisers advertises advertising adverts advice advices advisability advisable advise advised advisedly adviser advisers advises advising advisory advocacy advocate advocated advocates advocating adze aegean aegina aegis aeolian aeon aeons aerate aerated aerates aerating aeration aerator aerial aerially aerials aerify aerobatic aerobatics aerobe aerobes aerobic aerobically aerobics aerobraking aerodrome aerodromes aerodynamic aerodynamically aerodynamics aerofoil aerofoils aeronaut aeronautic aeronautical aeronautics aeroplane aeroplanes aerosol aerosols aerospace aesop aesthete aesthetes aesthetic aesthetically aestheticism aestheticsy afar affability affable affably affair affairs affect affectation affectations affected affectedly affecting affection affectionate affectionately affections affective affects afferent affidavit affidavits affiliate affiliated affiliates affiliating affiliation affiliations affine affinities affinity affirm affirmation affirmations affirmative affirmatively affirmed affirming affirms affix affixed affixes affixing afflict afflicted afflicting affliction afflictions afflicts affluence affluent afflux afford affordability affordable afforded affording affords afforestation afforested affray affront affronted affronts afghan afghani afghans afield afire aflame afloat afoot aforementioned aforesaid aforethought afraid afresh africa african africans afro afros aft after afterbirth aftercare aftereffect aftereffects afterglow afterlife afterlives aftermath afternoon afternoons aftershave aftershocks ";
		listOfData += "aftertaste afterthought afterthoughts afterward afterwards aga again against agakhan agape agar agaragar agave agaves age aged ageing ageings ageism ageless agencies agency agenda agendas agendums agent agents ageold ages agglomerated agglomerating agglomeration agglomerations agglutinative aggravate aggravated aggravates aggravating aggravation aggravations aggregate aggregated aggregates aggregating aggregation aggregations aggression aggressions aggressive aggressively aggressiveness aggressor aggressors aggrieved aggrievedly aghast agile agiler agility aging agings agio agitate agitated agitatedly agitates agitating agitation agitations agitator agitators agitprop agleam aglow agnostic agnosticism agnostics ago agog agonies agonise agonised agonises agonising agonisingly agonist agonists agony agora agoraphobia agoraphobic agouti agrarian agree agreeable agreeableness agreeably agreed agreeing agreement agreements agrees agribusiness agricultural agriculturalist agriculturalists agriculturally agriculture agrimony agrochemical agrochemicals agronomist agronomists agronomy aground ague ah aha ahead ahem ahoy aid aide aided aidedecamp aider aiders aides aidesdecamp aiding aids ail aileron ailerons ailing ailment ailments ails aim aimed aimer aiming aimless aimlessly aimlessness aims aint air airbase airborne airbrush airbus airconditioned airconditioner airconditioning aircraft aircrew aircrews aire aired airfield airfields airflow airforce airframe airframes airgun airier airiest airily airiness airing airings airless airlift airlifted airlifting airlifts airline airliner airliners airlines airlock airlocks airmail airman airmen airplane airplay airport airports airraid airs airship airships airsick airsickness airspace airstream airstrip airstrips airtight airtime airwave airwaves airway airways airworthiness airworthy airy aisle aisles aitches ajar akimbo akin ala alabama alabaster alacarte alack alacrity aladdin alanine alarm alarmed alarming alarmingly alarmism alarmist alarms alas alaska alaskan alb albania albany albatross albatrosses albeit albinism albino album albumen albumin albums alchemical alchemist alchemists alchemy alcohol alcoholic alcoholics alcoholism alcohols alcove alcoves aldehyde aldehydes alder alderman aldermen aldrin ale alehouse alembic alert alerted alerting alertly alertness alerts ales alfalfa alfatah alga algae algal algebra algebraic algebraical algebraically algebraist algebras algeria algerian algiers algorithm algorithmic algorithmically algorithms alias aliases alibaba alibi alibis alien alienate alienated alienates alienating alienation aliened aliening aliens alight alighted alighting alights align aligned aligning alignment alignments aligns alike alimentary alimony aline alined alines alining aliphatic aliquot aliquots alive alkali alkalic alkaline alkalinity alkalis alkalise alkaloid alkaloids alkanes alkyl all allay allayed allaying allays allegation allegations allege alleged allegedly alleges allegiance allegiances alleging allegorical allegorically allegories allegory allegri allegro allele alleles allelic allergen allergens allergic allergies allergy alleviate alleviated alleviates alleviating alleviation alleviations alley alleys alleyway alleyways alliance alliances allied allies alligator alligators alliterate alliterated alliterating alliteration alliterations alliterative allocatable allocate allocated allocates allocating allocation allocations allocator allocators allophones allot allotment allotments allotrope allotropic allots allotted allotting allow allowable allowance allowances allowed allowing allows alloy alloyed alloying alloys allude alluded alludes alluding allure allured allurement allurements allures alluring alluringly allusion allusions allusive alluvia alluvial alluvium ally allying almanac almanacs almighty almond almonds almost alms almshouse almshouses aloe aloes aloft aloha alone aloneness along alongside aloof aloofness aloud alp alpaca alpacas alpha alphabet alphabetic alphabetical alphabetically alphabets alphanumeric alphas alpine alps already alright also alt altar altarpiece altarpieces altars alter alterable alteration alterations altercate altercation altercations altered alterego altering alternate alternated alternately alternates alternating alternation alternations alternative alternatively alternatives alternator alternators alters although altimeter altimeters altitude altitudes alto altogether altruism altruist altruistic altruistically alts alum aluminium aluminum alumni alumnus alveolar alveoli always am amalgam amalgamate amalgamated amalgamates amalgamating amalgamation amalgamations amalgams amanuensis amass amassed amasses amassing amateur amateurish amateurishly amateurishness amateurism amateurs amatory amaze amazed amazement amazes amazing amazingly amazon amazons ambassador ambassadorial ambassadors amber ambergris ambiance ambidextrous ambience ambient ambiguities ambiguity ambiguous ambiguously ambit ";
		listOfData += "ambition ambitions ambitious ambitiously ambivalence ambivalent ambivalently amble ambled ambler ambles ambling ambrosia ambulance ambulances ambulant ambulate ambulatory ambuscade ambuscades ambush ambushed ambushers ambushes ambushing ameliorate ameliorated ameliorates ameliorating amelioration amen amenability amenable amend amendable amended amending amendment amendments amends amenities amenity amenorrhoea amens america american americans americium amethyst amethystine amethysts amiability amiable amiableness amiably amicability amicable amicably amid amide amidships amidst amigo amine amines amino amir amiss amity amman ammeter ammeters ammo ammonia ammonites ammonium ammunition amnesia amnesiac amnesic amnesties amnesty amniotic amoeba amoebae amoebic amok among amongst amoral amorality amorist amorous amorously amorphous amortisation amortise amortised amount amounted amounting amounts amour amours amp ampere amperes ampersand ampersands amphetamine amphetamines amphibia amphibian amphibians amphibious amphitheatre amphitheatres amphora ample ampler amplification amplifications amplified amplifier amplifiers amplifies amplify amplifying amplitude amplitudes amply ampoules amps ampule ampules ampuls amputate amputated amputating amputation amputations amputee amputees amuck amulet amulets amuse amused amusement amusements amuses amusing amusingly an ana anabolic anachronism anachronisms anachronistic anachronistically anaconda anacondas anaemia anaemic anaerobic anaerobically anaesthesia anaesthetic anaesthetics anaesthetise anaesthetised anaesthetising anaesthetist anaesthetists anagram anagrammatic anagrammatically anagrams anal analgesia analgesic analgesics anally analogical analogies analogise analogous analogously analogue analogues analogy analysable analyse analysed analyser analysers analyses analysing analysis analyst analysts analytic analytical analytically anamorphic ananas anaphora anaphoric anarchic anarchical anarchism anarchist anarchistic anarchists anarchy anathema anatomic anatomical anatomically anatomies anatomist anatomists anatomy ancestor ancestors ancestral ancestries ancestry anchor anchorage anchorages anchored anchoring anchorite anchors anchovies anchovy ancient anciently ancients ancillary and andante andes andrew androgynous android androids anecdotal anecdotally anecdote anecdotes anechoic anemia anemic anemone anemones anergy aneroid aneurysm aneurysms anew angel angelic angelica angels angelus anger angered angering angers angina anginal angioplasty angle angled anglepoise angler anglers angles anglian anglican angling angola angolan angolans angora angoras angrier angriest angrily angry angst angstroms anguish anguished anguishes angular angularity anhydrous anil aniline animal animals animate animated animatedly animates animating animation animations animator animators animism animist animists animosities animosity animus anion anionic anions anise aniseed aniseeds anisotropic anisotropies anisotropy ankara ankle ankles anklet anklets anna annal annals anneal annealed annealer annealing annex annexation annexations annexe annexed annexes annexing annihilate annihilated annihilates annihilating annihilation anniversaries anniversary annotate annotated annotates annotating annotation annotations announce announced announcement announcements announcer announcers announces announcing annoy annoyance annoyances annoyed annoyer annoyers annoying annoyingly annoys annual annualised annually annuals annuities annuity annul annular annuli annulled annulling annulment annuls annulus annunciation anode anodes anodised anodyne anoint anointed anointing anoints anomalies anomalous anomalously anomaly anomic anon anonym anonymity anonymous anonymously anonyms anorak anoraks anorexia anorexic another answer answerable answered answerer answering answers ant antacid antacids antagonise antagonised antagonises antagonising antagonism antagonisms antagonist antagonistic antagonists ante anteater anteaters antecedent antecedents antechamber antedate antedates antedating antediluvian antelope antelopes antenatal antenna antennae antennas anterior anteriorly anteroom anthem anthems anther anthologies anthologise anthologised anthology anthracite anthrax anthropic anthropocentric anthropogenic anthropogenically anthropoid anthropological anthropologist anthropologists anthropology anthropometric anthropomorphic anthropomorphising anthropomorphism anti antiabortionists antiaircraft antibiotic antibiotics antibodies antibody antic anticipate anticipated anticipates anticipating anticipation anticipations anticipative anticipatory anticlimax anticlockwise anticoagulants anticonstitutional antics anticyclone antidepressant antidepressants antidote antidotes antifreeze antigen antigenic antigens antihistamines antilope antimatter antimony antioxidants antiparticles antipathetic antipathies antipathy antipodes antiquarian antiquarianism antiquarians antiquaries antiquary antiquated antique ";
		listOfData += "antiques antiquities antiquity antiseptic antiseptics antisocial antistatic antisymmetric antisymmetry antitheses antithesis antithetic antithetical antithetically antitrust antiviral antler antlers antlion antlions antonym antonyms antral antrum ants antwerp anus anvil anvils anxieties anxiety anxious anxiously any anybody anyhow anymore anyone anyplace anything anyway anyways anywhere aorist aorta aortas aortic apace apache apaches apart apartment apartments apartness apathetic apathetically apathy ape aped apeman aperies aperiodic aperiodically aperitif aperitifs aperture apertures apery apes apex aphasia aphelion aphid aphids aphorism aphorisms aphorist aphoristic aphrodisiac aphrodisiacs apian apiaries apiarist apiary apiece aping apis apish aplenty aplomb apnea apnoea apocalypse apocalyptic apocryphal apogee apolitical apollo apologetic apologetically apologia apologies apologise apologised apologises apologising apologist apologists apology apoplectic apoplexy apostasy apostate apostates apostle apostles apostolate apostolic apostrophe apostrophes apostrophised apothecaries apothecary apotheosis appal appalled appalling appallingly appals apparatchik apparatchiks apparatus apparatuses apparel apparelled apparent apparently apparition apparitions appeal appealed appealing appealingly appeals appear appearance appearances appeared appearing appears appease appeased appeasement appeaser appeasers appeases appeasing appellant appellants appellate appellation appellations append appendage appendages appended appendices appendicitis appending appendix appends appertain appertained appertaining appetiser appetising appetite appetites applaud applauded applauding applauds applause apple applecart applepie apples applet appliance appliances applicability applicable applicant applicants application applications applicative applicator applicators applied applier applies applique apply applying appoint appointed appointee appointees appointing appointment appointments appoints apportion apportioned apportioning apportionment apportions apposite apposition appraisal appraisals appraise appraised appraisees appraiser appraisers appraises appraising appraisingly appreciable appreciably appreciate appreciated appreciates appreciating appreciation appreciations appreciative appreciatively apprehend apprehended apprehending apprehends apprehension apprehensions apprehensive apprehensively apprentice apprenticed apprentices apprenticeship apprenticeships apprise apprised apprising appro approach approachability approachable approached approaches approaching approbation appropriate appropriated appropriately appropriateness appropriates appropriating appropriation appropriations approval approvals approve approved approves approving approvingly approximate approximated approximately approximates approximating approximation approximations apricot apricots april apriori apron aprons apropos apse apses apsis apt aptest aptitude aptitudes aptly aptness aqua aqualung aquamarine aquanaut aquaria aquarium aquariums aquatic aquatics aqueduct aqueducts aqueous aquifer aquifers aquiline arab arabesque arabesques arabia arabian arabians arabic arable arabs arachnid arachnids arachnoid arachnophobia arak araks ararat arbiter arbiters arbitrage arbitrageur arbitrageurs arbitral arbitrarily arbitrariness arbitrary arbitrate arbitrated arbitrates arbitrating arbitration arbitrations arbitrator arbitrators arbor arboreal arboretum arbour arc arcade arcades arcadia arcading arcana arcane arcanely arcaneness arced arch archaeological archaeologically archaeologist archaeologists archaeology archaeopteryx archaic archaism archaisms archangel archangels archbishop archbishops archdeacon archdeaconry archdeacons archdiocese archduke archdukes arched archenemies archenemy archer archers archery arches archetypal archetype archetypes archetypical arching archipelago architect architectonic architects architectural architecturally architecture architectures architrave architraves archival archive archived archives archiving archivist archivists archly archness archway archways arcing arcs arctic ardency ardent ardently ardour arduous are area areal areas arena arenas arent argent argon argot arguable arguably argue argued arguer arguers argues arguing argument argumentation argumentative argumentatively arguments argus aria arias arid aridity aridness aright arise arisen arises arising aristocracies aristocracy aristocrat aristocratic aristocrats arithmetic arithmetical arithmetically arizona ark arkansas arks arm armada armadas armadillo armament armaments armature armatures armband armbands armchair armchairs armed armenia armful armfuls armhole armholes armies arming armistice armless armlet armlets armour armoured armourer armourers armouries armourplated armoury armpit armpits armrest arms army aroma aromas aromatherapist aromatherapy aromatic aromaticity aromatics arose around arousal arousals arouse aroused arouses arousing arrange ";
		listOfData += "arrangeable arranged arrangement arrangements arranger arranges arranging arrant arrases array arrayed arraying arrays arrears arrest arrestable arrested arrester arresting arrests arrhythmia arrival arrivals arrive arrived arriver arrives arriving arrogance arrogant arrogantly arrow arrowed arrowhead arrowheads arrowing arrowroot arrows arsenal arsenals arsenic arsenide arson arsonist arsonists art artefact artefacts artefactual arterial arteries artery artful artfully artfulness arthritic arthritis arthropod arthropods arthur artichoke artichokes article articled articles articulacy articular articulate articulated articulately articulates articulating articulation articulations articulatory artier artifice artificial artificiality artificially artillery artisan artisans artist artiste artistes artistic artistically artistry artists artless artlessly artlessness arts artwork artworks arty arum as asbestos asbestosis ascend ascendancy ascendant ascended ascendency ascender ascending ascends ascension ascensions ascent ascents ascertain ascertainable ascertained ascertaining ascertainment ascertains ascetic asceticism ascetics ascorbic ascribable ascribe ascribed ascribes ascribing ascription ascriptions aseptic asexual ash ashamed ashamedly ashbin ashbins ashcans ashen ashes ashore ashtray ashtrays ashy asia asian asians asiatic aside asides asinine ask askance asked askers askew asking asks aslant asleep asocial asp asparagus aspect aspects asperity aspersion aspersions asphalt asphyxia asphyxiate asphyxiated asphyxiation aspic aspidistra aspirant aspirants aspirate aspirated aspirates aspirating aspiration aspirational aspirations aspirators aspire aspired aspires aspirin aspiring aspirins asps ass assail assailable assailant assailants assailed assailing assails assassin assassinate assassinated assassinating assassination assassinations assassins assault assaulted assaulting assaults assay assayed assayer assays assegai assegais assemblage assemblages assemble assembled assembler assemblers assembles assemblies assembling assembly assent assented assenting assents assert asserted asserting assertion assertions assertive assertively assertiveness asserts asses assess assessable assessed assesses assessing assessment assessments assessor assessors asset assets assiduity assiduous assiduously assign assignable assignation assignations assigned assignees assigner assigning assignment assignments assigns assimilable assimilate assimilated assimilates assimilating assimilation assist assistance assistant assistants assisted assisting assists assizes associate associated associates associateship associating association associational associations associative associatively associativity assonance assort assorted assortment assortments assuage assuaged assuages assuaging assume assumed assumes assuming assumption assumptions assurance assurances assure assured assuredly assures assuring assyria assyrian aster asterisk asterisked asterisks astern asteroid asteroids asters asthma asthmatic asthmatics astigmatic astigmatism astir astonish astonished astonishes astonishing astonishingly astonishment astound astounded astounding astoundingly astounds astraddle astral astrally astray astride astringent astrolabe astrolabes astrologer astrologers astrological astrology astronaut astronautical astronautics astronauts astronomer astronomers astronomic astronomical astronomically astronomy astrophysical astrophysicist astrophysicists astrophysics astute astutely astuteness asunder aswan asylum asylums asymmetric asymmetrical asymmetrically asymmetries asymmetry asymptomatic asymptote asymptotes asymptotic asymptotically asynchronous asynchronously at atavism atavistic ate atelier atheism atheist atheistic atheistically atheists athena athens atherosclerosis athlete athletes athletic athletically athleticism athletics atlanta atlantic atlantis atlas atlases atmosphere atmospheres atmospheric atmospherically atmospherics atoll atolls atom atombomb atomic atomically atomicity atomisation atomised atomistic atoms atonal atonality atone atoned atonement atones atonic atoning atop atrial atrium atrocious atrociously atrocities atrocity atrophied atrophies atrophy atrophying atropine attach attachable attache attached attaches attaching attachment attachments attack attacked attacker attackers attacking attacks attain attainable attained attaining attainment attainments attains attempt attempted attempting attempts attend attendance attendances attendant attendants attended attendees attender attenders attending attends attention attentional attentions attentive attentively attentiveness attenuate attenuated attenuates attenuating attenuation attenuator attenuators attest attestation attested attesting attests attic attics attila attire attired attiring attitude attitudes attitudinal attorney attorneys attract attracted attracting attraction attractions attractive attractively attractiveness attractor attractors attracts ";
		listOfData += "attributable attribute attributed attributes attributing attribution attributions attributive attrition attritional attune attuned atypical atypically aubergine aubergines auburn auction auctioned auctioneer auctioneers auctioning auctions audacious audaciously audacity audibility audible audibly audience audiences audio audiovisual audit audited auditing audition auditioned auditioning auditions auditive auditor auditorium auditors auditory audits auger augers augite augment augmentation augmentations augmented augmenting augments augur augured augurs augury august augustus auk auks aunt auntie aunties aunts aupair aupairs aura aural aurally auras aurevoir auric auriculas aurora aurorae auroral auroras auspice auspices auspicious auspiciously aussie aussies austere austerely austerity austral australian austria autarchy auteur authentic authentically authenticate authenticated authenticates authenticating authentication authenticator authenticators authenticity author authored authoress authorial authoring authorisation authorisations authorise authorised authorises authorising authoritarian authoritarianism authoritarians authoritative authoritatively authorities authority authors authorship autism autistic auto autobahn autobahns autobiographical autobiographically autobiographies autobiography autocracies autocracy autocrat autocratic autocratically autocrats autocue autograph autographed autographing autographs autoignition autoimmune automat automata automate automated automates automatic automatically automatics automating automation automaton automats automobile automorphism automorphisms automotive autonomic autonomous autonomously autonomy autopilot autopsies autopsy autosuggestion autumn autumnal autumns auxiliaries auxiliary avail availabilities availability available availed availing avails avalanche avalanches avalanching avantgarde avarice avaricious avariciousness ave avenge avenged avenger avengers avenges avenging avens avenue avenues aver average averaged averagely averages averaging averred averring avers averse aversion aversions aversive avert averted averting averts avian aviaries aviary aviate aviation aviator aviators avid avidity avidly avionics avocado avoid avoidable avoidance avoided avoiding avoids avoirdupois avow avowal avowals avowed avowedly avowing avulsion avuncular await awaited awaiting awaits awake awaken awakened awakening awakenings awakens awakes awaking award awarded awarding awards aware awareness awash away awe awed aweless awesome awesomely awesomeness awestruck awful awfully awfulness awhile awkward awkwardest awkwardly awkwardness awls awn awning awnings awoke awoken awol awry axe axed axehead axeheads axeman axes axial axially axillary axing axiom axiomatic axiomatically axiomatising axioms axis axle axles axolotl axon axons aye ayurvedic azalea azaleas azimuth azimuthal azores aztec aztecs azure ";
	}

	std::string next()
	{
		size_t nextPosition = listOfData.find( ' ', position );
		size_t count = nextPosition == std::string::npos ?
			std::string::npos : nextPosition - position;
		std::string temp = listOfData.substr( position, count );
		position = nextPosition == std::string::npos ? 0 : nextPosition + 1;
		return temp;
	}

	std::string listOfData;
	size_t position;
};


struct OldTestListModel::Implementation
{
	Implementation( OldTestListModel& self, bool shortList );
	~Implementation();

	char* copyString( const std::string& s ) const;
	void generateData();
	void clear();
	int columnCount();

	OldTestListModel& self_;
	std::vector<OldTestListItem*> items_;
	StringList2 dataSource_;
	bool shortList_;
	std::vector<std::string> headerText_;
	std::vector<std::string> footerText_;
};


OldTestListModel::Implementation::Implementation( OldTestListModel& self, bool shortList )
	: self_( self )
	, shortList_( shortList )
{
	generateData();

	headerText_.push_back( "Random Words" );

	if (columnCount() == 2)
	{
		headerText_.push_back( "Second Column" );
		footerText_.push_back( "The" );
		footerText_.push_back( "End" );
	}
	else
	{
		footerText_.push_back( "The End" );
	}
}


OldTestListModel::Implementation::~Implementation()
{
	clear();
}


char* OldTestListModel::Implementation::copyString(
	const std::string& s ) const
{
	char* itemData = new char[s.length() + 1];
	memcpy( itemData, s.data(), s.length() );
	itemData[s.length()] = 0;
	return itemData;
}


void OldTestListModel::Implementation::generateData()
{
	std::string dataString = dataSource_.next();
	std::random_device randomDevice;
	std::default_random_engine randomEngine( randomDevice() );
	std::uniform_int_distribution<size_t> uniformDistribution( 0, 999999 );
	size_t max = shortList_ ? 1000 : 2000;

	while (!dataString.empty())
	{
		if (!shortList_ && items_.size() % 3 == 0)
		{
			size_t colour = uniformDistribution( randomEngine );
			items_.push_back( new OldTestListItem(
				dataString.c_str(), colour ) );
		}
		else
		{
			items_.push_back( new OldTestListItem(
				dataString.c_str(), dataString.c_str() ) );
		}
		
		dataString = dataSource_.next();

		if (--max == 0)
		{
			break;
		}
	}
}


void OldTestListModel::Implementation::clear()
{
	for (auto& item: items_)
	{
		delete item;
	}

	items_.clear();
}


int OldTestListModel::Implementation::columnCount()
{
	return shortList_ ? 1 : 2;
}


OldTestListModel::OldTestListModel( bool shortList )
	: impl_( new Implementation( *this, shortList ) )
{
}


OldTestListModel::OldTestListModel( const OldTestListModel& rhs )
	: impl_( new Implementation( *this, rhs.impl_->shortList_ ) )
{
}


OldTestListModel::~OldTestListModel()
{
}


OldTestListModel& OldTestListModel::operator=( const OldTestListModel& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this, rhs.impl_->shortList_ ) );
	}

	return *this;
}


IItem* OldTestListModel::item( size_t index ) const
{
	return impl_->items_.at( index );
}


size_t OldTestListModel::index( const IItem* item ) const
{
	auto& items = impl_->items_;
	auto itr = std::find( items.begin(), items.end(), item );
	return itr == items.end() ? -1 : itr - items.begin();
}


bool OldTestListModel::empty() const
{
	return impl_->items_.empty();
}


size_t OldTestListModel::size() const
{
	return impl_->items_.size();
}


int OldTestListModel::columnCount() const
{
	return impl_->columnCount();
}


Variant OldTestListModel::getData( int column, size_t roleId ) const
{
	if (column >= columnCount())
	{
		return Variant();
	}

	if (roleId == headerTextRole::roleId_)
	{
		return impl_->headerText_[column].c_str();
	}
	else if (roleId == footerTextRole::roleId_)
	{
		return impl_->footerText_[column].c_str();
	}

	return Variant();
}


bool OldTestListModel::setData( int column, size_t roleId, const Variant & data )
{
	return false;
}


/////////////////////////////////////////////////////////
struct TestListModel::Implementation
{
	Implementation( TestListModel& self, bool shortList );
	~Implementation();

	char* copyString( const std::string& s ) const;
	void generateData();
	void clear();
	int columnCount();

	TestListModel& self_;
	std::vector<TestListItem*> items_;
	StringList2 dataSource_;
	bool shortList_;
	std::vector<std::string> headerText_;
	std::vector<std::string> footerText_;
	//std::vector<TestListModel::DataCallback> preItemChangeHandlers;
	//std::vector<TestListModel::DataCallback> postItemChangeHandlers;
};


TestListModel::Implementation::Implementation( TestListModel& self, bool shortList )
	: self_( self )
	, shortList_( shortList )
{
	generateData();

	headerText_.push_back( "Random Words" );

	if (columnCount() == 2)
	{
		headerText_.push_back( "Second Column" );
		footerText_.push_back( "The" );
		footerText_.push_back( "End" );
	}
	else
	{
		footerText_.push_back( "The End" );
	}
}


TestListModel::Implementation::~Implementation()
{
	clear();
}


char* TestListModel::Implementation::copyString(
	const std::string& s ) const
{
	char* itemData = new char[s.length() + 1];
	memcpy( itemData, s.data(), s.length() );
	itemData[s.length()] = 0;
	return itemData;
}


void TestListModel::Implementation::generateData()
{
	std::string dataString = dataSource_.next();
	std::random_device randomDevice;
	std::default_random_engine randomEngine( randomDevice() );
	std::uniform_int_distribution<size_t> uniformDistribution( 0, 999999 );
	size_t max = shortList_ ? 100 : 1000;

	while (!dataString.empty())
	{
		if (!shortList_ && items_.size() % 3 == 0)
		{
			size_t colour = uniformDistribution( randomEngine );
			items_.push_back( new TestListItem(
				dataString.c_str(), colour ) );
		}
		else
		{
			items_.push_back( new TestListItem(
				dataString.c_str(), dataString.c_str() ) );
		}
		
		dataString = dataSource_.next();

		if (--max == 0)
		{
			break;
		}
	}
}


void TestListModel::Implementation::clear()
{
	for (auto& item: items_)
	{
		delete item;
	}

	items_.clear();
}


int TestListModel::Implementation::columnCount()
{
	return shortList_ ? 1 : 2;
}


TestListModel::TestListModel( bool shortList )
	: impl_( new Implementation( *this, shortList ) )
{
}


TestListModel::TestListModel( const TestListModel& rhs )
	: impl_( new Implementation( *this, rhs.impl_->shortList_ ) )
{
}


TestListModel::~TestListModel()
{
}


TestListModel& TestListModel::operator=( const TestListModel& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this, rhs.impl_->shortList_ ) );
	}

	return *this;
}


Variant TestListModel::getData( int row, int column, size_t roleId ) const
{
	if (column >= columnCount())
	{
		return Variant();
	}

    auto role = static_cast< int >( roleId );
	if (role == headerTextRole::roleId_)
	{
		return impl_->headerText_[column].c_str();
	}
	else if (role == footerTextRole::roleId_)
	{
		return impl_->footerText_[column].c_str();
	}

	return Variant();
}


bool TestListModel::setData( int row, int column, size_t roleId, const Variant & data )
{
	return false;
}


AbstractItem * TestListModel::item( int row ) const
{
	return impl_->items_.at( size_t( row ) );
}


int TestListModel::index( const AbstractItem * item ) const
{
	auto& items = impl_->items_;
	auto itr = std::find( items.begin(), items.end(), item );
	return itr == items.end() ? -1 : int( itr - items.begin() );
}


int TestListModel::rowCount() const
{
	return (int)impl_->items_.size();
}


int TestListModel::columnCount() const
{
	return impl_->columnCount();
}


Connection TestListModel::connectPreItemDataChanged( DataCallback callback )
{
	return Connection();
}


Connection TestListModel::connectPostItemDataChanged( DataCallback callback )
{
	return Connection();
}
} // end namespace wgt
