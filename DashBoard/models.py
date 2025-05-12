from django.db import models

class MemberInfo(models.Model):
    name = models.CharField(max_length=4)                   # HG_NM
    hanja_name = models.CharField(max_length=4)             # HJ_NM
    birth_date = models.CharField(max_length=20, blank=True, null=True)  # BIRDY_DT
    party = models.CharField(max_length=30)                 # POLY_NM
    district = models.CharField(max_length=30)              # ORIG_NM
    elected_type = models.CharField(max_length=20)          # ELECT_GBN_NM
    reelection = models.CharField(max_length=10)            # REELE_GBN_NM
    gender = models.CharField(max_length=3)                 # SEX_GBN_NM
    phone = models.CharField(max_length=50)                 # TEL_NO
    email = models.CharField(max_length=50)                 # NAAS_EMAIL_ADDR
    position = models.CharField(max_length=30)              # JOB_RES_NM
    main_committee = models.CharField(max_length=30)        # CMIT_NM
    member_code = models.CharField(max_length=30)           # MONA_CD
    election_unit = models.CharField(max_length=50)         # UNITS
    total_bills  = models.PositiveIntegerField(default=0)
    passed_bills = models.PositiveIntegerField(default=0)
    pass_rate    = models.FloatField(default=0.0)
    
    def __str__(self):
        return self.name
    
class MemberImageInfo(models.Model):
     member = models.OneToOneField(MemberInfo, on_delete=models.CASCADE, related_name='MemberImageInfo')
     image_url = models.URLField(max_length=300)
