from django.db import models

class MemberInfo(models.Model):
    name = models.CharField(max_length=100)                   # HG_NM
    hanja_name = models.CharField(max_length=200)             # HJ_NM
    birth_date = models.CharField(max_length=200, blank=True, null=True)  # BIRDY_DT
    party = models.CharField(max_length=200)                 # POLY_NM
    district = models.CharField(max_length=200)              # ORIG_NM
    elected_type = models.CharField(max_length=200)          # ELECT_GBN_NM
    reelection = models.CharField(max_length=200)            # REELE_GBN_NM
    gender = models.CharField(max_length=200)                 # SEX_GBN_NM
    phone = models.CharField(max_length=200)                 # TEL_NO
    email = models.CharField(max_length=200)                 # NAAS_EMAIL_ADDR
    position = models.CharField(max_length=200)              # JOB_RES_NM
    main_committee = models.CharField(max_length=200)        # CMIT_NM
    member_code = models.CharField(max_length=100, unique=True) # MONA_CD
    election_unit = models.CharField(max_length=200)         # UNITS
    total_bills  = models.PositiveIntegerField(default=0)
    passed_bills = models.PositiveIntegerField(default=0)
    pass_rate = models.FloatField(default=0.0)
    attendance_rate = models.FloatField(null=True, blank=True)  # 출석률 추가
    
    def __str__(self):
        return self.name
    
class MemberImageInfo(models.Model):
     member = models.OneToOneField(MemberInfo, on_delete=models.CASCADE, related_name='MemberImageInfo')
     image_url = models.URLField(max_length=300)
